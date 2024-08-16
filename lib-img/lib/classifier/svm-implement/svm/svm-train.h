#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <cstdio>
#include <omp.h> // Incluye la cabecera de OpenMP
#include "svm.h"
#define Malloc(type, n) (type *)malloc((n) * sizeof(type))

void exit_input_error(int line_num)
{
	fprintf(stderr, "Wrong input format at line %d\n", line_num);
	exit(1);
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
void read_problem(const char *filename);
void do_cross_validation();

struct svm_parameter param;
struct svm_problem prob;
struct svm_model *model;
struct svm_node *x_space;
int cross_validation;
int nr_fold;

static char *line1 = NULL;
static int max_line_len1;

void train_model(const char *fileTrain, const char *fileModel, double gamma, double c)
{
	const char *anrchivo_train = fileTrain;
	const char *model_file_name = fileModel;

	param.svm_type = 0;
	param.kernel_type = 2;
	param.degree = 3;
	param.gamma = gamma;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = c;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	cross_validation = 0;
	read_problem(anrchivo_train);

	model = svm_train(&prob, &param);
	svm_save_model(model_file_name, model);
	svm_free_and_destroy_model(&model);

	svm_destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line1);
}

static char *readline1(FILE *input)
{
	int len;

	if (fgets(line1, max_line_len1, input) == NULL)
		return NULL;

	while (strrchr(line1, '\n') == NULL)
	{
		max_line_len1 *= 2;
		line1 = (char *)realloc(line1, max_line_len1);
		len = (int)strlen(line1);
		if (fgets(line1 + len, max_line_len1 - len, input) == NULL)
			break;
	}
	return line1;
}

void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double, prob.l);

	svm_cross_validation(&prob, &param, nr_fold, target);

	if (param.svm_type == EPSILON_SVR ||
		param.svm_type == NU_SVR)
	{
#pragma omp parallel
		{
			double loc_total_error = 0, loc_sumv = 0, loc_sumy = 0, loc_sumvv = 0, loc_sumyy = 0, loc_sumvy = 0;

#pragma omp for
			for (i = 0; i < prob.l; i++)
			{
				double y = prob.y[i];
				double v = target[i];
				loc_total_error += (v - y) * (v - y);
				loc_sumv += v;
				loc_sumy += y;
				loc_sumvv += v * v;
				loc_sumyy += y * y;
				loc_sumvy += v * y;
			}

#pragma omp atomic
			total_error += loc_total_error;

#pragma omp atomic
			sumv += loc_sumv;

#pragma omp atomic
			sumy += loc_sumy;

#pragma omp atomic
			sumvv += loc_sumvv;

#pragma omp atomic
			sumyy += loc_sumyy;

#pragma omp atomic
			sumvy += loc_sumvy;
		}

		printf("Cross Validation Mean squared error = %g\n", total_error / prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
			   ((prob.l * sumvy - sumv * sumy) * (prob.l * sumvy - sumv * sumy)) /
				   ((prob.l * sumvv - sumv * sumv) * (prob.l * sumyy - sumy * sumy)));
	}
	else
	{
#pragma omp parallel
		{
			int loc_total_correct = 0;

#pragma omp for
			for (i = 0; i < prob.l; i++)
				if (target[i] == prob.y[i])
					++loc_total_correct;

#pragma omp atomic
			total_correct += loc_total_correct;
		}

		printf("Cross Validation Accuracy = %g%%\n", 100.0 * total_correct / prob.l);
	}
	free(target);
}

void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	size_t elements, j;
	FILE *fp = fopen(filename, "r");
	char *endptr;
	char *idx, *val, *label;

	if (fp == NULL)
	{
		fprintf(stderr, "can't open input file %s\n", filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;

	max_line_len1 = 1024;
	line1 = Malloc(char, max_line_len1);

	// Primero contaremos el número de líneas y elementos
	while (readline1(fp) != NULL)
	{
		char *p = strtok(line1, "\t");

		while (1)
		{
			p = strtok(NULL, "\t");

			if (p == NULL || *p == '\n')
				break;

			++elements;
		}
		++elements;
		++prob.l;
	}

	rewind(fp);

	prob.y = Malloc(double, prob.l);
	prob.x = Malloc(struct svm_node *, prob.l);
	x_space = Malloc(struct svm_node, elements);

	max_index = 0;
	j = 0;

#pragma omp parallel
	{
#pragma omp for
		for (i = 0; i < prob.l; i++)
		{
			int inst_max_index = -1;
			readline1(fp);
			prob.x[i] = &x_space[j];
			label = strtok(line1, " \t\n");
			if (label == NULL)
				exit_input_error(i + 1);

			prob.y[i] = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				exit_input_error(i + 1);

			while (1)
			{
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;

				errno = 0;
				x_space[j].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
					exit_input_error(i + 1);
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					exit_input_error(i + 1);

				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;
			x_space[j++].index = -1;
		}
	}

	if (param.gamma == 0 && max_index > 0)
		param.gamma = 1.0 / max_index;

	if (param.kernel_type == PRECOMPUTED)
		for (i = 0; i < prob.l; i++)
		{
			if (prob.x[i][0].index != 0)
			{
				fprintf(stderr, "Wrong input format: first column must be 0:sample_serial_number\n");
				exit(1);
			}
			if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			{
				fprintf(stderr, "Wrong input format: sample_serial_number out of range\n");
				exit(1);
			}
		}

	fclose(fp);
}