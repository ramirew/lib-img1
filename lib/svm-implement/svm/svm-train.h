
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <cstdio>
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

/**
 * @note Este archivo fue modificado y adaptado para que se ejecute como una funcion.
 * Los cambios que se hicieron es eliminar las funciones dedicadas a capturar y procesar 
 * los paramatros ingresados por terminal.
 * Se renombro la funcion main por train_model.
 * IMPORTANTE: No se modifico ninguna funcion principal que afecte a su funcionamiento, solo se adapto
 * para que funcione al ser llamado como una funcion.
*/

//void print_null(const char *s) {}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
void read_problem(const char *filename);
void do_cross_validation();

struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;
int cross_validation;
int nr_fold;

static char *line1 = NULL;
static int max_line_len1;


void  train_model(const char* fileTrain, const char *fileModel )//estos es un void
{
    const char* anrchivo_train=fileTrain;
    const char* model_file_name=fileModel;

	param.svm_type = 0;
	param.kernel_type = 2;
	param.degree = 3;
	param.gamma = 29;	
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1000; //500
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	cross_validation = 0;
	read_problem(anrchivo_train);

		model = svm_train(&prob,&param);
		svm_save_model(model_file_name,model);
		svm_free_and_destroy_model(&model);

	svm_destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line1);

}


static char* readline1(FILE *input)
{
	int len;

	if(fgets(line1,max_line_len1,input) == NULL)
		return NULL;

	while(strrchr(line1,'\n') == NULL)
	{
		max_line_len1 *= 2;
		line1 = (char *) realloc(line1,max_line_len1);
		len = (int) strlen(line1);
		if(fgets(line1+len,max_line_len1-len,input) == NULL)
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
	double *target = Malloc(double,prob.l);

	svm_cross_validation(&prob,&param,nr_fold,target);
	if(param.svm_type == EPSILON_SVR ||
	   param.svm_type == NU_SVR)
	{
		for(i=0;i<prob.l;i++)
		{
			double y = prob.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
			((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
			((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			);
	}
	else
	{
		for(i=0;i<prob.l;i++)
			if(target[i] == prob.y[i])
				++total_correct;
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
	}
	free(target);
}


// read in a problem (in svmlight format)

void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	size_t elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;

	max_line_len1 = 1024;
	line1 = Malloc(char,max_line_len1);



	while(readline1(fp)!=NULL)
	{

		char *p = strtok(line1,"\t"); // label

		// features
		
		while(1)
		{
			p = strtok(NULL,"\t");
			
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
				
			++elements;
		}
		++elements;
		++prob.l;
		
	}


	rewind(fp);

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,elements);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline1(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line1," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}

	if(param.gamma == 0 && max_index > 0)
		param.gamma = 1.0/max_index;

	if(param.kernel_type == PRECOMPUTED)
		for(i=0;i<prob.l;i++)
		{
			if (prob.x[i][0].index != 0)
			{
				fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
				exit(1);
			}
			if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			{
				fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
				exit(1);
			}
		}

	fclose(fp);
}
