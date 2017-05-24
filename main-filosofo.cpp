#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <math.h>
#include <sys/time.h> 
#include <sys/resource.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <pthread.h>


using namespace std;
using namespace BlackLib;

volatile int speedRed = 1, speedYellow = 1, speedGreen = 1;
pthread_mutex_t mutex[3];
pthread_cond_t cond[3];

void *redTrain(void* param){

	//led vermelho
	BlackGPIO ledRed1(GPIO_26, output, SecureMode); //vermelho/amarelo
	BlackGPIO ledRed2(GPIO_47, output, SecureMode); //vermelho/verde
	BlackGPIO ledRed3(GPIO_65, output, SecureMode);
	//3-2-1

	while(true){
		
		ledRed3.setValue(high);
		usleep(speedRed);
		pthread_mutex_lock(&mutex[0]);
		pthread_mutex_lock(&mutex[2]);
		ledRed3.setValue(low);

		ledRed2.setValue(high);
		usleep(speedRed);
		ledRed2.setValue(low);
		//pthread_cond_signal(&cond[0]);
		
		//pthread_cond_wait(&cond[2], &mutex[2]);
		ledRed1.setValue(high);
		usleep(speedRed);
		pthread_mutex_unlock(&mutex[0]);
		pthread_mutex_unlock(&mutex[2]);
		ledRed1.setValue(low);
		


	}

}

void *yellowTrain(void* param){

	//led amarelo
	BlackGPIO ledYellow1(GPIO_44, output, SecureMode); //amarelo/verde
	BlackGPIO ledYellow2(GPIO_61, output, SecureMode); //amarelo/vermelho
	BlackGPIO ledYellow3(GPIO_22, output, SecureMode);
	//3-2-1

	while(true){
		
		ledYellow3.setValue(high);
		usleep(speedYellow);
		pthread_mutex_lock(&mutex[1]);
		pthread_mutex_lock(&mutex[2]);
		ledYellow3.setValue(low);

	
		ledYellow2.setValue(high);
		usleep(speedYellow);
		ledYellow2.setValue(low);
		//pthread_cond_signal(&cond[2]);
		
		//pthread_cond_wait(&cond[1], &mutex[1]);
		ledYellow1.setValue(high);
		usleep(speedYellow);
		pthread_mutex_unlock(&mutex[1]);
		pthread_mutex_unlock(&mutex[2]);
		ledYellow1.setValue(low);
		
		
	}
}

void *greenTrain(void* param){


	
	//led verde
	BlackGPIO ledGreen1(GPIO_45, output, SecureMode); //verde/amarelo
	BlackGPIO ledGreen2(GPIO_46, output, SecureMode); //verde/vermelho
	BlackGPIO ledGreen3(GPIO_27, output, SecureMode);
	// 3-1-2

	while(true){
		
		
		ledGreen3.setValue(high);
		usleep(speedGreen);
		pthread_mutex_lock(&mutex[0]);
		pthread_mutex_lock(&mutex[1]);
		ledGreen3.setValue(low);

		/*regiao critica*/
		
		ledGreen1.setValue(high);
		usleep(speedGreen);
		
		
		ledGreen1.setValue(low);
		//pthread_cond_signal(&cond[1]);
		
		
		
		//pthread_cond_wait(&cond[0], &mutex[0]);
		ledGreen2.setValue(high);
		usleep(speedGreen);
		pthread_mutex_unlock(&mutex[0]);
		pthread_mutex_unlock(&mutex[1]);
		ledGreen2.setValue(low);
		
		/*regiao critica*/

	}
}

int main(int argc, char * argv[])
{/* main thread */

	/*gpios
	44, 45, 26, 47, 46, 27, 65, 22, 61 */
	
	/*potenciometros*/
	ADC potRed(AIN5);
	ADC potYellow(AIN3);	
	ADC potGreen(AIN1);

	float valuePotRed, valuePotYellow, valuePotGreen;

	/*mutex e cond*/
	for(int i=0; i<3; i++){
		pthread_mutex_init(&mutex[i], 0);
    	//pthread_cond_init(&cond[i], 0);
	}
	
	/*child threads*/
	pthread_t red_t, yellow_t, green_t;
	int res;
	res = pthread_create(&red_t, NULL, redTrain, NULL);
    if (res != 0) {
        perror("Criação da Thread falhou");
        exit(EXIT_FAILURE);
    }
	res = pthread_create(&yellow_t, NULL, yellowTrain, NULL);
	if (res != 0) {
        perror("Criação da Thread falhou");
        exit(EXIT_FAILURE);
    }
	res = pthread_create(&green_t, NULL, greenTrain, NULL);
	if (res != 0) {
        perror("Criação da Thread falhou");
        exit(EXIT_FAILURE);
    }

	while(true){
		valuePotRed = potRed.getPercentValue();
		speedRed = 60000 + 10000*valuePotRed;
		valuePotYellow = potYellow.getPercentValue(); 
		speedYellow = 60000 + 10000*valuePotYellow;
		valuePotGreen = potGreen.getPercentValue();
		speedGreen = 60000 + 10000*valuePotGreen;
		usleep(1000);
	}
		

    return 0;
}
