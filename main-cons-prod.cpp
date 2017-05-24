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

volatile int speedConsumidor = 1, speedProdutor = 1;
volatile bool buffer = false;
pthread_mutex_t mutex;
pthread_cond_t cond;


void *produtor(void* param){

	//led amarelo
	BlackGPIO ledGreen1(GPIO_45, output, SecureMode); //produzindo
	BlackGPIO ledGreen2(GPIO_46, output, SecureMode); //esperando_para_produzir

	while(true){
		
		
		if(buffer==false){
			pthread_mutex_lock(&mutex);
			ledGreen2.setValue(low);
			ledGreen1.setValue(high);
			buffer = true;
			usleep(speedProdutor);
			ledGreen1.setValue(low);
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
		}
		
		ledGreen2.setValue(high);
		usleep(speedProdutor);
		
		
	}
}

void *consumidor(void* param){


	
	//led vermelho
	BlackGPIO ledRed1(GPIO_26, output, SecureMode); //consumindo
	BlackGPIO ledRed2(GPIO_47, output, SecureMode); //esperando_para_consumir
	

	while(true){
		pthread_mutex_lock(&mutex);
		while(buffer == false){
			//esperando pra consumir
			ledRed2.setValue(high);
			usleep(speedConsumidor);
			
			pthread_cond_wait(&cond, &mutex);
		}
		ledRed2.setValue(low);
		ledRed1.setValue(high);
		buffer = false;
		usleep(speedConsumidor);
		ledRed1.setValue(low);
		pthread_mutex_unlock(&mutex);		
	
	}
}

int main(int argc, char * argv[])
{/* main thread */

	
	/*potenciometros*/
	ADC potConsumidor(AIN5);
	ADC potProdutor(AIN3);	
	BlackGPIO ledBuffer(GPIO_44, output, SecureMode);
	float valuePotConsumidor, valuePotProdutor;

	/*mutex e cond*/
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&cond, 0);
	
	/*child threads*/
	pthread_t produtor_t, consumidor_t;
	int res;
	res = pthread_create(&produtor_t, NULL, produtor, NULL);
    if (res != 0) {
        perror("Criação da Thread falhou");
        exit(EXIT_FAILURE);
    }
	res = pthread_create(&consumidor_t, NULL, consumidor, NULL);
	if (res != 0) {
        perror("Criação da Thread falhou");
        exit(EXIT_FAILURE);
    }

	while(true){

		if(buffer == true){
			ledBuffer.setValue(high);
		}else{
			ledBuffer.setValue(low);
		}
		valuePotConsumidor = potConsumidor.getPercentValue();
		speedConsumidor = 60000 + 10000*valuePotConsumidor;
		valuePotProdutor = potProdutor.getPercentValue(); 
		speedProdutor = 60000 + 10000*valuePotProdutor;
		usleep(1000);
	}
		

    return 0;
}
