
/*
* Autor: Ing. Ismael Farinango - 2023
* web: https://ismaelfari1996.github.io/ismael-portafolio/
*/
# include <fstream>
#include <vector>
using namespace std;
typedef int **cannyMatrix;
class Canny{
private:
	GaussianFilter gaussian;
	StbImageImplementation stb;
	Utility u;
	double  horizontalPixelDifference;
	double  verticalPixelDifference;
	double  diagonalDiffernce, reverseDiagonalDifference;
	double x,y; 
	double computeSigma(int kernelSize=5);
	void computeGradient(double **image, int width, int height, double ** &gradient, double** &gradientOrientation );
	//void addGamma(double ** &image, int width, int height, double gamma);
	void getMinMaxValue(double **image, int width, int height, double &minValue, double &maxValue);
	double ** nonMaximunSuppression(double ** &gradient, double ** &gradientOrientation, int width, int height, double radius);
	void fixOrientation(double ** &gradientOrientation,int width, int height);
	int  **hysteresis(double ** image, int width, int height, double thresh[2]);
	void blackWitheVector(double ** image, int width, int height, double * &blackWithe, int * &stack, std::vector<int> &pixel, double minthresh);
	int ** getBorder(double * blackWitheVector, int width, int height);
public:
	/**
	 * @brief Funcion de correccion de gamma en la imagen.
	 * @param image Matriz puntero de tipo double** que contiene la imagen
	 * @param width Numero de columnas de la imagen
	 * @param height Numero de filas de la imagen
	 * @param gamma Valor de Gamma a aplicar
	*/
	void addGamma(double ** &image, int width, int height, double gamma);
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
	cannyMatrix canny(double **image, int width, int height, double thresh[2],double sigma,double gamma=1, double radius=1);

};

double Canny::computeSigma(int kernelSize){
	return 0.3 * ((kernelSize - 1) * 0.5 - 1) + 0.8;
}

void Canny::computeGradient(double **image, int width, int height, double ** &gradient, double** &gradientOrientation){
	/*
	* Esta funcion calcula los valores de gradiente y la orientacion del gradiente.
	* gradient y gradientOrientation son parametros(matrices) pasados por referencia, se inicializa en esta funcion.
	*/
	gradient=this->u.initMatrix(width,height,0.0);
	gradientOrientation=this->u.initMatrix(width,height,0.0);
	double auxOrientation;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			this->horizontalPixelDifference=(j+1)==width?0:image[i][j+1] - (j==0?0:image[i][j-1]);
			//printf("%f\t",((i+1)==height?0:image[i+1][j]) - (i==0?0:image[i-1][j]));
			this->verticalPixelDifference=(i+1)==height?0:image[i+1][j] - (i==0?0:image[i-1][j]);
			this->diagonalDiffernce= ((i+1==height|| j+1==width)?0:image[i+1][j+1])- ((i==0||j==0)?0:image[i-1][j-1]);
			this->reverseDiagonalDifference=((i==0||j+1==width)?0:image[i-1][j+1])-((j==0||i+1==height)?0:image[i+1][j-1]);
			x = ( horizontalPixelDifference + (diagonalDiffernce + reverseDiagonalDifference)/2.0 ) * 1;
			y = ( verticalPixelDifference + (diagonalDiffernce - reverseDiagonalDifference)/2.0 ) * 1;
			gradient[i][j]= sqrt((x * x) +(y * y)); 
			auxOrientation=atan2(-y,x);
			auxOrientation=auxOrientation<0?((auxOrientation + M_PI) * 1):(auxOrientation * 1);
			gradientOrientation[i][j]=(auxOrientation*180)/M_PI;

		}
		
	}
	
}

void Canny::fixOrientation(double ** &gradientOrientation,int width, int height){
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			gradientOrientation[i][j]=double(int(gradientOrientation[i][j]))+1;
		}
	}
}


void Canny::blackWitheVector(double ** image, int width, int height, double * &blackWithe, int * &stack, std::vector<int> &pixel, double minthresh){
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
	blackWithe=new double[width*height];
	stack=new int [width*height];
	int cont=0;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			blackWithe[cont]=image[j][i];
			stack[cont]=0;
			if (blackWithe[cont]>minthresh)
				pixel.push_back(cont+1);
			cont++;
		}
	}
}

int ** Canny::getBorder(double * blackWitheVector, int width, int height){
	/*
	* Transforma el vector binario en una matriz binaria. 
	*/
	cannyMatrix border=this->u.initMatrix(width, height,0);
	int index_row = 0;
	int index_col = 0;
	for (int i = 0; i < (width*height); ++i)
	{
		blackWitheVector[i]=(blackWitheVector[i]==-1)?1:0;
		if (index_row == height){
			index_row = 0;
			index_col++;
		}

		if (index_col == width){
			break;
		}
		border[index_row][index_col] = int(blackWitheVector[i]);
		index_row++;

	}
	delete [] blackWitheVector;
	return border;
}

int ** Canny::hysteresis(double ** image, int width, int height, double thresh[2]){
	/*
	* Clasifica los pixeles como parte de los bordes y no bordes, en base a los limites 
	* de los umbrales proporcionados como parametro.
	* thresh[0]: umbral minimo
	* thresh[1]; umbral maximo.
	*/
	int rc = width * height;
	int rcmr = rc - height;
	int rp1 = height + 1;
	int cont = 0;
	double *bw;
	std::vector<int> pix; 
	int *stack;
	//Inicializa y calcula los valores de bw, stack, pix a partir de la matriz imagen y el umbral minimo.
	blackWitheVector(image,width,height,bw,stack,pix,thresh[0]);
	int npix = pix.size();
	for (int i = 0; i < npix; ++i)
	{
		stack[i] = pix[i];
		bw[pix[i] - 1] = -1;		
	}

	int O[] = {-1, 1, - height-1, - height, - height+1, height-1, height, height+1};	
    npix=npix-1;
	while (npix != 0){
		int v = stack[npix];
		npix--;
		if ((v > rp1) && (v < rcmr))
		{
			for (int l = 0; l < 8; ++l)
			{	
				int ind = O[l] + v;
				if (bw[ind-1] > thresh[1])
				{
					npix++;
					stack[npix] = ind;
					bw[ind-1] = -1;
				}
				
			}
		}
	}
	delete[] stack;
	u.free_memory(image,height);
	pix.clear();
	return getBorder(bw,width,height);
}

double ** Canny::nonMaximunSuppression(double ** &gradient, double ** &gradientOrientation, int width, int height, double radius){
	/*
	* Refina los bordes en base al radio proporcionado.
	* Tiene como objetivo preservar los pixeles que correspondan a los maximos locales.
	*/
	double iradius = ceil(radius);
	double xoff[181];
	double yoff[181];
	double hfrac[181];
	double vfrac[181];
	stb_image result=this->u.initMatrix(width,height,0.0);
	for (int i = 0; i <= 180; ++i)
	{
		double angle = ((i * M_PI )/ 180);
		xoff[i] = radius * cos(angle);
		yoff[i] = radius * sin(angle);

		hfrac[i] = xoff[i] - floor(xoff[i]);
		vfrac[i] = yoff[i] - floor(yoff[i]);

	}

	fixOrientation(gradientOrientation, width, height);
	for (int i = int(iradius); i < int(height - iradius) ; ++i)
	{
	for (int j = int(iradius); j < int(width - iradius); ++j)
	{
		int or1 = int(gradientOrientation[i][j]);
		double x = double(j+ xoff[or1-1]+1);
		double y = double(i - yoff[or1 - 1 ] + 1);
		
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
		 x = double((j+1) -xoff[or1-1]);
		 y = double(i + yoff[or1 - 1 ] + 1);
		
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
				result[i][j] = gradient[i][j];

		}
	}
}

return result;
}

void Canny::getMinMaxValue(double **image, int width, int height, double &minValue, double &maxValue){
	minValue=255;
	maxValue=0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			minValue=minValue>=image[i][j]?image[i][j]:minValue;
		}
	}
		for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j]=image[i][j]- minValue;
			maxValue=(image[i][j]>=maxValue)?image[i][j]:maxValue;
		}
	}
}


void Canny::addGamma(double ** &image, int width, int height, double gamma){
	/*
	* Agrega gamma a la imagen con el fin de aumentar o disminuir la intensidad de los pixeles.
	*/
	double minValue;
	double maxValue;
	getMinMaxValue(image, width, height,minValue, maxValue);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			image[i][j]=pow(image[i][j],(1/gamma));
		}
	}
}
cannyMatrix Canny::canny(double **image, int width, int height, double thresh[2],double sigma, double gamma, double radius){
	/*
	* Los valores sigma, gamma y radius se ajustan de acuerdo al problema.
	*/
	//Se aplica el filtro gaussiano
	image =this->gaussian.gaussianFilter(image,width,height,sigma);
	// Declaracion de la variable gradient y gradientOrientation
	stb_image gradient, gradientOrientation;
	//Calculo de la gradiente.
	computeGradient(image,width, height, gradient, gradientOrientation);
	// Agrega gamma
	addGamma(gradient,width,height,gamma);
	// supresion no maxima en base al radio
    stb_image nonMaxSupr=nonMaximunSuppression(gradient,gradientOrientation,width,height,radius);
    // liberacion de punteros
	this->u.free_memory(gradient,height);
	this->u.free_memory(gradientOrientation,height);
	//canny_free_memory(nonMaxSupr,height);
	this->u.free_memory(image,height);
	printf("\n Devloped by Ismael Farinango -2023 \n");
	return hysteresis(nonMaxSupr,width,height,thresh);
}