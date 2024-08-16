#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <omp.h> // Incluye OpenMP
#include "svm.h"

int print_null(const char *s, ...) { return 0; }

static int (*info2)(const char *fmt, ...) = &printf;

struct svm_node *x;
int max_nr_attr = 64;

struct svm_model *model2;
int predict_probability = 0;

static char *line2 = NULL;
static int max_line_len2;

static char *readline2(FILE *input)
{
	int len;

	if (fgets(line2, max_line_len2, input) == NULL)
		return NULL;

	while (strrchr(line2, '\n') == NULL)
	{
		max_line_len2 *= 2;
		line2 = (char *)realloc(line2, max_line_len2);
		len = (int)strlen(line2);
		if (fgets(line2 + len, max_line_len2 - len, input) == NULL)
			break;
	}
	return line2;
}

void exit_input_error2(int line_num2)
{
	fprintf(stderr, "Wrong input format at line %d\n", line_num2);
	exit(1);
}

void predict(FILE *input, FILE *output)
{
	int svm_type = svm_get_svm_type(model2);
	int nr_class = svm_get_nr_class(model2);
	double *prob_estimates = NULL;

	if (predict_probability)
	{
		if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
			info2("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n", svm_get_svr_probability(model2));
		else
		{
			int *labels = (int *)malloc(nr_class * sizeof(int));
			svm_get_labels(model2, labels);
			prob_estimates = (double *)malloc(nr_class * sizeof(double));
			fprintf(output, "labels");
			for (int j = 0; j < nr_class; j++)
				fprintf(output, " %d", labels[j]);
			fprintf(output, "\n");
			free(labels);
		}
	}

	max_line_len2 = 1024;
	line2 = (char *)malloc(max_line_len2 * sizeof(char));

	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;
	int correct = 0;
	int total = 0;

#pragma omp parallel shared(input, output, prob_estimates, svm_type, nr_class) private(x, line2, max_nr_attr)
	{
		// Variables privadas para cada hilo
		double local_error = 0;
		double local_sump = 0, local_sumt = 0, local_sumpp = 0, local_sumtt = 0, local_sumpt = 0;
		int local_correct = 0;
		int local_total = 0;

		while (1)
		{
			char *line = readline2(input);
			if (line == NULL)
				break;

			int i = 0;
			double target_label, predict_label;
			char *idx, *val, *label, *endptr;
			int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

			label = strtok(line, " \t\n");
			if (label == NULL) // empty line
				exit_input_error2(total + 1);

			target_label = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				exit_input_error2(total + 1);

			while (1)
			{
				if (i >= max_nr_attr - 1) // need one more for index = -1
				{
#pragma omp critical
					{
						max_nr_attr *= 2;
						x = (struct svm_node *)realloc(x, max_nr_attr * sizeof(struct svm_node));
					}
				}

				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;

				errno = 0;
				x[i].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
					exit_input_error2(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					exit_input_error2(total + 1);

				++i;
			}
			x[i].index = -1;

			if (predict_probability && (svm_type == C_SVC || svm_type == NU_SVC))
			{
				predict_label = svm_predict_probability(model2, x, prob_estimates);
#pragma omp critical
				{
					fprintf(output, "%g", predict_label);
					for (int j = 0; j < nr_class; j++)
						fprintf(output, " %g", prob_estimates[j]);
					fprintf(output, "\n");
				}
			}
			else
			{
				predict_label = svm_predict(model2, x);
#pragma omp critical
				{
					fprintf(output, "%.17g\n", predict_label);
				}
			}

			if (predict_label == target_label)
				++local_correct;
			local_error += (predict_label - target_label) * (predict_label - target_label);
			local_sump += predict_label;
			local_sumt += target_label;
			local_sumpp += predict_label * predict_label;
			local_sumtt += target_label * target_label;
			local_sumpt += predict_label * target_label;
			++local_total;
		}

#pragma omp critical
		{
			error += local_error;
			sump += local_sump;
			sumt += local_sumt;
			sumpp += local_sumpp;
			sumtt += local_sumtt;
			sumpt += local_sumpt;
			correct += local_correct;
			total += local_total;
		}
	}

	if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
	{
		info2("Mean squared error = %g (regression)\n", error / total);
		info2("Squared correlation coefficient = %g (regression)\n",
			  ((total * sumpt - sump * sumt) * (total * sumpt - sump * sumt)) /
				  ((total * sumpp - sump * sump) * (total * sumtt - sumt * sumt)));
	}
	else
		info2("Accuracy = %g%% (%d/%d) (classification)\n",
			  (double)correct / total * 100, correct, total);

	if (predict_probability)
		free(prob_estimates);
}

void evalue(const char *testFile, const char *modelFile, const char *resultFile)
{
	FILE *input, *output;

	const char *test_file = testFile;
	const char *modelo = modelFile;
	const char *resultado = resultFile;

	input = fopen(test_file, "r");
	if (input == NULL)
		exit(1);

	output = fopen(resultado, "w");
	if (output == NULL)
		exit(1);

	if ((model2 = svm_load_model(modelo)) == 0)
		exit(1);

	x = (struct svm_node *)malloc(max_nr_attr * sizeof(struct svm_node));
	if (predict_probability)
	{
		if (svm_check_probability_model(model2) == 0)
		{
			fprintf(stderr, "Model does not support probability estimates\n");
			exit(1);
		}
	}
	else
	{
		if (svm_check_probability_model(model2) != 0)
			info2("Model supports probability estimates, but disabled in prediction.\n");
	}

	predict(input, output);
	svm_free_and_destroy_model(&model2);
	free(x);
	free(line2);
	fclose(input);
	fclose(output);
}