
/*
 * Autor: Ing. Ismael Farinango - 2023
 * web: https://ismaelfari1996.github.io/ismael-portafolio/
 */
#include <fstream>
#include <vector>
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

/**
 * Calcula los valores de gradiente y la orientación del gradiente.
 *
 * @param image La matriz de la imagen de entrada.
 * @param width El ancho de la imagen.
 * @param height La altura de la imagen.
 * @param gradient La matriz de salida para almacenar los valores de gradiente.
 * @param gradientOrientation La matriz de salida para almacenar la orientación del gradiente.
 */
void Canny::computeGradient(double **image, int width, int height, double **&gradient, double **&gradientOrientation)
{
	/*
	 * Esta funcion calcula los valores de gradiente y la orientacion del gradiente.
	 * gradient y gradientOrientation son parametros(matrices) pasados por referencia, se inicializa en esta funcion.
	 */
	gradient = this->u.initMatrix(width, height, 0.0);
	gradientOrientation = this->u.initMatrix(width, height, 0.0);
	double auxOrientation;

#pragma omp parallel for private(auxOrientation) collapse(2)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			double horizontalPixelDifference = (j + 1) == width ? 0 : image[i][j + 1] - (j == 0 ? 0 : image[i][j - 1]);
			double verticalPixelDifference = (i + 1) == height ? 0 : image[i + 1][j] - (i == 0 ? 0 : image[i - 1][j]);
			double diagonalDiffernce = ((i + 1 == height || j + 1 == width) ? 0 : image[i + 1][j + 1]) - ((i == 0 || j == 0) ? 0 : image[i - 1][j - 1]);
			double reverseDiagonalDifference = ((i == 0 || j + 1 == width) ? 0 : image[i - 1][j + 1]) - ((j == 0 || i + 1 == height) ? 0 : image[i + 1][j - 1]);

			double x = (horizontalPixelDifference + (diagonalDiffernce + reverseDiagonalDifference) / 2.0) * 1;
			double y = (verticalPixelDifference + (diagonalDiffernce - reverseDiagonalDifference) / 2.0) * 1;

			gradient[i][j] = sqrt((x * x) + (y * y));
			auxOrientation = atan2(-y, x);
			auxOrientation = auxOrientation < 0 ? ((auxOrientation + M_PI) * 1) : (auxOrientation * 1);
			gradientOrientation[i][j] = (auxOrientation * 180) / M_PI;
		}
	}
}

/**
 * @brief Corrige la orientación de los gradientes.
 *
 * Esta función se utiliza para corregir la orientación de los gradientes en una imagen.
 * Recibe como parámetros un puntero a una matriz de doble puntero que representa los gradientes de la imagen,
 * el ancho y alto de la imagen.
 *
 * La función itera sobre cada píxel de la imagen y ajusta la orientación del gradiente sumando 1 al valor actual.
 *
 * @param gradientOrientation Puntero a una matriz de doble puntero que representa los gradientes de la imagen.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 */
void Canny::fixOrientation(double **&gradientOrientation, int width, int height)
{
#pragma omp parallel for collapse(2)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			gradientOrientation[i][j] = double(int(gradientOrientation[i][j])) + 1;
		}
	}
}

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
void Canny::blackWitheVector(double **image, int width, int height, double *&blackWithe, int *&stack, std::vector<int> &pixel, double minthresh)
{
	blackWithe = new double[width * height];
	stack = new int[width * height];

#pragma omp parallel
	{
		std::vector<int> local_pixel;
#pragma omp for
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				int cont = i * height + j;
				blackWithe[cont] = image[j][i];
				stack[cont] = 0;
				if (blackWithe[cont] > minthresh)
				{
					local_pixel.push_back(cont + 1);
				}
			}
		}

#pragma omp critical
		pixel.insert(pixel.end(), local_pixel.begin(), local_pixel.end());
	}
}

/**
 * @brief Obtiene una matriz binaria a partir de un vector binario.
 *
 * @param blackWitheVector El vector binario.
 * @param width El ancho de la matriz.
 * @param height La altura de la matriz.
 * @return int** La matriz binaria resultante.
 */

int **Canny::getBorder(double *blackWitheVector, int width, int height)
{
	/*
	 * Transforma el vector binario en una matriz binaria.
	 */
	cannyMatrix border = this->u.initMatrix(width, height, 0);

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < (width * height); ++i)
		{
			blackWitheVector[i] = (blackWitheVector[i] == -1) ? 1 : 0;
		}

#pragma omp for collapse(2)
		for (int index_row = 0; index_row < height; ++index_row)
		{
			for (int index_col = 0; index_col < width; ++index_col)
			{
				int i = index_row * width + index_col;
				border[index_row][index_col] = int(blackWitheVector[i]);
			}
		}
	}

	delete[] blackWitheVector;
	return border;
}

/**
 * Aplica el algoritmo de histeresis para detectar los bordes en una imagen utilizando el método de Canny.
 *
 * @param image La imagen de entrada representada como una matriz de valores de tipo double.
 * @param width El ancho de la imagen.
 * @param height La altura de la imagen.
 * @param thresh Un arreglo de tamaño 2 que contiene los umbrales de detección de bordes.
 *              El primer valor del arreglo es el umbral inferior y el segundo valor es el umbral superior.
 * @return Una matriz de enteros que representa los bordes detectados en la imagen.
 */
int **Canny::hysteresis(double **image, int width, int height, double thresh[2])
{
	int rc = width * height;
	int rcmr = rc - height;
	int rp1 = height + 1;
	int cont = 0;
	double *bw = new double[rc];
	std::vector<int> pix;
	int *stack = new int[rc];

	// Inicialización del vector pix y stack
	int npix = pix.size();
#pragma omp parallel for
	for (int i = 0; i < npix; ++i)
	{
		stack[i] = pix[i];
		bw[pix[i] - 1] = -1;
	}

	int O[] = {-1, 1, -height - 1, -height, -height + 1, height - 1, height, height + 1};
	npix = npix - 1;

#pragma omp parallel
	{
#pragma omp single
		{
			while (npix >= 0)
			{
				int v = stack[npix];
				npix--;
				if ((v > rp1) && (v < rcmr))
				{
#pragma omp parallel for
					for (int l = 0; l < 8; ++l)
					{
						int ind = O[l] + v;
						if (bw[ind - 1] > thresh[1])
						{
#pragma omp critical
							{
								npix++;
								stack[npix] = ind;
								bw[ind - 1] = -1;
							}
						}
					}
				}
			}
		}
	}

	delete[] stack;
	u.free_memory(image, height);
	pix.clear();
	return getBorder(bw, width, height);
}

/**
 * Realiza la supresión de no máximos en los bordes detectados.
 * Esta función tiene como objetivo preservar los píxeles que correspondan a los máximos locales.
 *
 * @param gradient Matriz de gradientes de la imagen.
 * @param gradientOrientation Matriz de orientaciones de los gradientes de la imagen.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 * @param radius Radio utilizado para refinar los bordes.
 * @return Matriz con los bordes después de la supresión de no máximos.
 */
double **Canny::nonMaximunSuppression(double **&gradient, double **&gradientOrientation, int width, int height, double radius)
{
	/*
	 * Refina los bordes en base al radio proporcionado.
	 * Tiene como objetivo preservar los píxeles que correspondan a los máximos locales.
	 */
	double iradius = ceil(radius);
	double xoff[181];
	double yoff[181];
	double hfrac[181];
	double vfrac[181];
	double **result = this->u.initMatrix(width, height, 0.0);

	// Inicialización de offsets y fracciones
	for (int i = 0; i <= 180; ++i)
	{
		double angle = ((i * M_PI) / 180);
		xoff[i] = radius * cos(angle);
		yoff[i] = radius * sin(angle);

		hfrac[i] = xoff[i] - floor(xoff[i]);
		vfrac[i] = yoff[i] - floor(yoff[i]);
	}

	fixOrientation(gradientOrientation, width, height);

#pragma omp parallel for
	for (int i = int(iradius); i < int(height - iradius); ++i)
	{
		for (int j = int(iradius); j < int(width - iradius); ++j)
		{
			int or1 = int(gradientOrientation[i][j]);
			double x = double(j + xoff[or1 - 1] + 1);
			double y = double(i - yoff[or1 - 1] + 1);

			int fx = int(floor(x));
			int cx = int(ceil(x));
			int fy = int(floor(y));
			int cy = int(ceil(y));

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

				fx = int(floor(x));
				cx = int(ceil(x));
				fy = int(floor(y));
				cy = int(ceil(y));

				tl = gradient[fy - 1][fx - 1];
				tr = gradient[fy - 1][cx - 1];
				bl = gradient[cy - 1][fx - 1];
				br = gradient[cy - 1][cx - 1];

				upperavg = tl + hfrac[or1 - 1] * (tr - tl);
				loweravg = bl + hfrac[or1 - 1] * (br - bl);
				double v2 = upperavg + vfrac[or1 - 1] * (loweravg - upperavg);

				if (gradient[i][j] > v2)
				{
					result[i][j] = gradient[i][j];
				}
			}
		}
	}

	return result;
}

/**
 * @brief Obtiene el valor mínimo y máximo de una matriz de imagen.
 *
 * Esta función recorre la matriz de imagen y encuentra los valores mínimo y máximo.
 * Los valores mínimo y máximo se actualizan en los parámetros de referencia minValue y maxValue.
 *
 * @param image La matriz de imagen.
 * @param width El ancho de la matriz de imagen.
 * @param height La altura de la matriz de imagen.
 * @param minValue El valor mínimo de la matriz de imagen (actualizado por la función).
 * @param maxValue El valor máximo de la matriz de imagen (actualizado por la función).
 */
void Canny::getMinMaxValue(double **image, int width, int height, double &minValue, double &maxValue)
{
	minValue = 255;
	maxValue = 0;

// Fase 1: Encontrar el valor mínimo
#pragma omp parallel
	{
		double localMin = 255;
		double localMax = 0;

#pragma omp for
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				localMin = std::min(localMin, image[i][j]);
				localMax = std::max(localMax, image[i][j]);
			}
		}

#pragma omp critical
		{
			minValue = std::min(minValue, localMin);
			maxValue = std::max(maxValue, localMax);
		}
	}

// Fase 2: Ajustar los valores de la imagen
#pragma omp parallel for collapse(2)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j] = image[i][j] - minValue;
#pragma omp critical
			{
				maxValue = std::max(maxValue, image[i][j]);
			}
		}
	}
}

/**
 * @brief Agrega gamma a la imagen con el fin de aumentar o disminuir la intensidad de los pixeles.
 *
 * @param image La imagen a la que se le aplicará el ajuste gamma.
 * @param width El ancho de la imagen.
 * @param height La altura de la imagen.
 * @param gamma El valor gamma que se utilizará para ajustar la intensidad de los pixeles.
 */
void Canny::addGamma(double **&image, int width, int height, double gamma)
{
	/*
	 * Agrega gamma a la imagen con el fin de aumentar o disminuir la intensidad de los píxeles.
	 */
	double minValue;
	double maxValue;
	getMinMaxValue(image, width, height, minValue, maxValue);

// Paralelizar la aplicación de la transformación gamma
#pragma omp parallel for collapse(2)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j] = std::pow(image[i][j], (1.0 / gamma));
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