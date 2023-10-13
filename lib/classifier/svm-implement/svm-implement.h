#include "./svm/svm-train.h"
#include "./svm/svm.h"
#include "./svm/svm.cpp"
#include "./svm/svm-predict.h"
class SvmImplement
{
private:

public:
    /**
    * @brief Entrenar modelo svm con los parametros por defecto
    * @param featureFile ruta del archivo  que contiene el vector de caracteristicas
    * @param modelOutFile ruta donde se va a guaradar el modelo
    */
    void svmTrain(const char* featureFile, const char *modelOutFile);

    /**
     * @brief Evalua el mmodelo entrenado
     * @param testFeatureFile ruta del archivo que contiene el vector de caracteristicas
     * @param modelFile ruta del archivo que contiene el modelo
     * @param resultFile ruta donde se va a guaradar los resultados
    */
    void svmTest(const char *testFeatureFile, const char* modelFile, const char* resultFile);
};

void SvmImplement::svmTrain(const char* featureFile, const char *modelOutFile){
    train_model(featureFile,modelOutFile);
}

void SvmImplement::svmTest(const char *testFeatureFile, const char* modelFile, const char* resultFile){
    evalue(testFeatureFile,modelFile,resultFile);
}
