
/*
 * Autor: Ing. Ismael Farinango - 2023
 * web: https://ismaelfari1996.github.io/ismael-portafolio/
 */
#include <fstream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <algorithm>
using namespace std;
typedef int **cannyMatrix;
class Canny
{
private:
	GaussianFilter gaussian;
	StbImageImplementation stb;
	Utility u;
	double horizontalPixelDifference;
	double verticalPixelDifference;
	double diagonalDiffernce, reverseDiagonalDifference;
	double x, y;
	double computeSigma(int kernelSize = 5);
	void computeGradient(double **image, int width, int height, double **&gradient, double **&gradientOrientation);
	// void addGamma(double ** &image, int width, int height, double gamma);
	void getMinMaxValue(double **image, int width, int height, double &minValue, double &maxValue);
	double **nonMaximunSuppression(double **&gradient, double **&gradientOrientation, int width, int height, double radius);
	void fixOrientation(double **&gradientOrientation, int width, int height);
	int **hysteresis(double **image, int width, int height, double thresh[2]);
	void blackWitheVector(double **image, int width, int height, double *&blackWithe, int *&stack, std::vector<int> &pixel, double minthresh);
	int **getBorder(double *blackWitheVector, int width, int height);

public:
	/**
	 * @brief Funcion de correccion de gamma en la imagen.
	 * @param image Matriz puntero de tipo double** que contiene la imagen
	 * @param width Numero de columnas de la imagen
	 * @param height Numero de filas de la imagen
	 * @param gamma Valor de Gamma a aplicar
	 */
	void addGamma(double **&image, int width, int height, double gamma);
	/**
	 * @brief Funcion Canny para la deteccion de bordes finos.
	 * @param image Matriz puntero de tipo double** que contiene la imagen.
	 * @param width Numero de columnas de la imagen
	 * @param height Numero de filas de la imagen
	 * @param thresh Umbrales minimos y maximos. Formato double [2]={0.0,0.5}
	 * @param sigma Valor de sigma.
	 * @param gamma Valor de gamma, por defecto gamma=1
	 * @param radius Valor del radio, por defecto radius=1
	 */
	cannyMatrix canny(double **image, int width, int height, double thresh[2], double sigma, double gamma = 1, double radius = 1);
};

double Canny::computeSigma(int kernelSize)
{
	return 0.3 * ((kernelSize - 1) * 0.5 - 1) + 0.8;
}

void Canny::computeGradient(double **image, int width, int height, double **&gradient, double **&gradientOrientation)
{
	/*
	 * Esta función calcula los valores de gradiente y la orientación del gradiente.
	 * gradient y gradientOrientation son parámetros (matrices) pasados por referencia, se inicializa en esta función.
	 */
	gradient = this->u.initMatrix(width, height, 0.0);
	gradientOrientation = this->u.initMatrix(width, height, 0.0);

	// Variables auxiliares para cálculo
	double auxOrientation;
	double horizontalPixelDifference, verticalPixelDifference, diagonalDiffernce, reverseDiagonalDifference;
	double x, y;

// Paralelización del bucle exterior con OpenMP
#pragma omp parallel for private(auxOrientation, horizontalPixelDifference, verticalPixelDifference, diagonalDiffernce, reverseDiagonalDifference, x, y)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			horizontalPixelDifference = (j + 1) == width ? 0 : image[i][j + 1] - (j == 0 ? 0 : image[i][j - 1]);
			verticalPixelDifference = (i + 1) == height ? 0 : image[i + 1][j] - (i == 0 ? 0 : image[i - 1][j]);
			diagonalDiffernce = ((i + 1 == height || j + 1 == width) ? 0 : image[i + 1][j + 1]) - ((i == 0 || j == 0) ? 0 : image[i - 1][j - 1]);
			reverseDiagonalDifference = ((i == 0 || j + 1 == width) ? 0 : image[i - 1][j + 1]) - ((j == 0 || i + 1 == height) ? 0 : image[i + 1][j - 1]);
			x = (horizontalPixelDifference + (diagonalDiffernce + reverseDiagonalDifference) / 2.0) * 1;
			y = (verticalPixelDifference + (diagonalDiffernce - reverseDiagonalDifference) / 2.0) * 1;
			gradient[i][j] = sqrt((x * x) + (y * y));
			auxOrientation = atan2(-y, x);
			auxOrientation = auxOrientation < 0 ? ((auxOrientation + M_PI) * 1) : (auxOrientation * 1);
			gradientOrientation[i][j] = (auxOrientation * 180) / M_PI;
		}
	}
}
void Canny::fixOrientation(double **&gradientOrientation, int width, int height)
{
// Paralelización del bucle exterior con OpenMP
#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			gradientOrientation[i][j] = double(int(gradientOrientation[i][j])) + 1;
		}
	}
}

void Canny::blackWitheVector(double **image, int width, int height, double *&blackWithe, int *&stack, std::vector<int> &pixel, double minthresh)
{
	/**
	 * Calcula el vector blackWithe, el vector stack y el vector pixel a partir de una imagen.
	 *
	 * @param image El puntero a la matriz de la imagen.
	 * @param width El ancho de la imagen.
	 * @param height La altura de la imagen.
	 * @param blackWithe El puntero al vector blackWithe.
	 *                   Se asignará memoria dinámicamente dentro de la función y deberá ser liberada por el llamador.
	 * @param stack El puntero al vector stack.
	 *              Se asignará memoria dinámicamente dentro de la función y deberá ser liberada por el llamador.
	 * @param pixel La referencia al vector pixel.
	 *              Se utilizará para almacenar los píxeles que superen el umbral mínimo.
	 *              Los píxeles se agregarán al final del vector.
	 * @param minthresh El umbral mínimo para considerar un píxel como blanco o negro.
	 */
	blackWithe = new double[width * height];
	stack = new int[width * height];

	int cont = 0;
#pragma omp parallel for
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int index = j * width + i;
			blackWithe[index] = image[j][i];
			stack[index] = 0;
			if (blackWithe[index] > minthresh)
			{
#pragma omp critical
				{
					pixel.push_back(index + 1);
				}
			}
		}
	}
}

int **Canny::getBorder(double *blackWitheVector, int width, int height)
{
	/*
	 * Transforma el vector binario en una matriz binaria.
	 */
	cannyMatrix border = this->u.initMatrix(width, height, 0);

// Paralelización del bucle principal con OpenMP
#pragma omp parallel for
	for (int i = 0; i < width * height; ++i)
	{
		int index_row = i % height;
		int index_col = i / height;

		blackWitheVector[i] = (blackWitheVector[i] == -1) ? 1 : 0;

		if (index_row < height && index_col < width)
		{
			border[index_row][index_col] = int(blackWitheVector[i]);
		}
	}

	delete[] blackWitheVector;
	return border;
}
int **Canny::hysteresis(double **image, int width, int height, double thresh[2])
{
	/*
	 * Clasifica los píxeles como parte de los bordes y no bordes, en base a los límites
	 * de los umbrales proporcionados como parámetro.
	 * thresh[0]: umbral mínimo
	 * thresh[1]; umbral máximo.
	 */
	int rc = width * height;
	int rcmr = rc - height;
	int rp1 = height + 1;
	double *bw;
	std::vector<int> pix;
	int *stack;

	// Inicializa y calcula los valores de bw, stack, pix a partir de la matriz imagen y el umbral mínimo.
	blackWitheVector(image, width, height, bw, stack, pix, thresh[0]);
	int npix = pix.size();

// Inicializar el stack
#pragma omp parallel for
	for (int i = 0; i < npix; ++i)
	{
		stack[i] = pix[i];
		bw[pix[i] - 1] = -1;
	}

	int O[] = {-1, 1, -height - 1, -height, -height + 1, height - 1, height, height + 1};
	npix = npix - 1;

// Procesar el stack
#pragma omp parallel
	{
		std::vector<int> local_stack;

#pragma omp for schedule(dynamic)
		for (int idx = 0; idx <= npix; ++idx)
		{
			local_stack.push_back(stack[idx]);
		}

		while (!local_stack.empty())
		{
			int v = local_stack.back();
			local_stack.pop_back();

			if ((v > rp1) && (v < rcmr))
			{
				for (int l = 0; l < 8; ++l)
				{
					int ind = O[l] + v;
					if (bw[ind - 1] > thresh[1])
					{
						local_stack.push_back(ind);
						bw[ind - 1] = -1;
					}
				}
			}
		}

#pragma omp critical
		{
			for (int val : local_stack)
			{
				stack[++npix] = val;
			}
		}
	}

	delete[] stack;
	u.free_memory(image, height);
	pix.clear();
	return getBorder(bw, width, height);
}

double **Canny::nonMaximunSuppression(double **&gradient, double **&gradientOrientation, int width, int height, double radius)
{
	/*
	 * Refina los bordes en base al radio proporcionado.
	 * Tiene como objetivo preservar los píxeles que correspondan a los máximos locales.
	 */
	double iradius = std::ceil(radius);
	double xoff[181];
	double yoff[181];
	double hfrac[181];
	double vfrac[181];
	stb_image result = this->u.initMatrix(width, height, 0.0);

	for (int i = 0; i <= 180; ++i)
	{
		double angle = ((i * M_PI) / 180);
		xoff[i] = radius * std::cos(angle);
		yoff[i] = radius * std::sin(angle);
		hfrac[i] = xoff[i] - std::floor(xoff[i]);
		vfrac[i] = yoff[i] - std::floor(yoff[i]);
	}

	fixOrientation(gradientOrientation, width, height);

// Paralelización del bucle exterior con OpenMP
#pragma omp parallel for
	for (int i = int(iradius); i < int(height - iradius); ++i)
	{
		for (int j = int(iradius); j < int(width - iradius); ++j)
		{
			int or1 = int(gradientOrientation[i][j]);
			double x = double(j + xoff[or1 - 1] + 1);
			double y = double(i - yoff[or1 - 1] + 1);
			int fx = int(std::floor(x));
			int cx = int(std::ceil(x));
			int fy = int(std::floor(y));
			int cy = int(std::ceil(y));
			double tl = gradient[fy - 1][fx - 1];
			double tr = gradient[fy - 1][cx - 1];
			double bl = gradient[cy - 1][fx - 1];
			double br = gradient[cy - 1][cx - 1];
			double upperavg = tl + hfrac[or1 - 1] * (tr - tl);
			double loweravg = bl + hfrac[or1 - 1] * (br - bl);
			double v1 = upperavg + vfrac[or1 - 1] * (loweravg - upperavg);

			if (gradient[i][j] > v1)
			{
				x = double((j + 1) - xoff[or1 - 1]);
				y = double(i + yoff[or1 - 1] + 1);
				fx = int(std::floor(x));
				cx = int(std::ceil(x));
				fy = int(std::floor(y));
				cy = int(std::ceil(y));
				tl = gradient[fy - 1][fx - 1];
				tr = gradient[fy - 1][cx - 1];
				bl = gradient[cy - 1][fx - 1];
				br = gradient[cy - 1][cx - 1];
				upperavg = tl + hfrac[or1 - 1] * (tr - tl);
				loweravg = bl + hfrac[or1 - 1] * (br - bl);
				double v2 = upperavg + vfrac[or1 - 1] * (loweravg - upperavg);

				if (gradient[i][j] > v2)
					result[i][j] = gradient[i][j];
			}
		}
	}

	return result;
}
void Canny::getMinMaxValue(double **image, int width, int height, double &minValue, double &maxValue)
{
	minValue = 255;
	maxValue = 0;

// Encontrar el valor mínimo en paralelo
#pragma omp parallel for reduction(min : minValue)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			minValue = std::min(minValue, image[i][j]);
		}
	}

// Ajustar la imagen y encontrar el valor máximo en paralelo
#pragma omp parallel for reduction(max : maxValue)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j] = image[i][j] - minValue;
			maxValue = std::max(maxValue, image[i][j]);
		}
	}
}

void Canny::addGamma(double **&image, int width, int height, double gamma)
{
	/*
	 * Agrega gamma a la imagen con el fin de aumentar o disminuir la intensidad de los píxeles.
	 */
	double minValue;
	double maxValue;
	getMinMaxValue(image, width, height, minValue, maxValue);

#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j] = pow(image[i][j], (1.0 / gamma));
		}
	}
}

cannyMatrix Canny::canny(double **image, int width, int height, double thresh[2], double sigma, double gamma, double radius)
{
	/*
	 * Los valores sigma, gamma y radius se ajustan de acuerdo al problema.
	 */
	// Se aplica el filtro gaussiano
	image = this->gaussian.gaussianFilter(image, width, height, sigma);
	// Declaracion de la variable gradient y gradientOrientation
	stb_image gradient, gradientOrientation;
	// Calculo de la gradiente.
	computeGradient(image, width, height, gradient, gradientOrientation);
	// Agrega gamma
	addGamma(gradient, width, height, gamma);
	// supresion no maxima en base al radio
	stb_image nonMaxSupr = nonMaximunSuppression(gradient, gradientOrientation, width, height, radius);
	// liberacion de punteros
	this->u.free_memory(gradient, height);
	this->u.free_memory(gradientOrientation, height);
	// canny_free_memory(nonMaxSupr,height);
	this->u.free_memory(image, height);
	printf("\n Devloped by Ismael Farinango -2023 \n");
	return hysteresis(nonMaxSupr, width, height, thresh);
}