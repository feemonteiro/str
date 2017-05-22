/**
 * Autor: Tiago Fernandes de Miranda
 * Universidade Federal do Rio Grande do Norte
 */

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

// Inclua as classes que achar necessario

using namespace std;
using namespace BlackLib;

void carga(int k)
{
  float f = 0.999999;
  for(int i=0; i<k*1.5; i++)
    {
      f = f*f*f*f*f;
      f = 1.56;
        for(int j=0; j<k; j++)
           {
	     f = sin(f)*sin(f)*f*f*f;

            }
    }

}

int main(int argc, char * argv[])
{
	pid_t pid1, pid2;
	ADC pot4(AIN4);
	ADC pot6(AIN6);	
	

	float pot4Value, pot6Value;
	//cout << "valor da prioridade do processo: " << getpriority(PRIO_PROCESS, 0 ) <<endl;

	pid1= fork();

	if(pid1 == -1){
		exit(1);
	}else if(pid1 == 0){
		//processo filho 1
		BlackGPIO led4(GPIO_30, output, SecureMode);
		led4.setValue(low);

		while(1){
			//cout << "Processo filho 1"<< endl;
			//cout << "valor da prioridade do processo filho1: " << getpriority(PRIO_PROCESS, getpid()) <<endl;
			carga(1000);
			led4.toggleValue();
		}

	}else{
		//processo pai

		pid2 = fork();
		if(pid2 == -1){
			exit(1);
		}else if(pid2 == 0){
			//processo filho 2
			BlackGPIO led6(GPIO_60, output, SecureMode);
			led6.setValue(low);

			while(1){
				//cout << "Processo filho 2"<< endl;
				//cout << "valor da prioridade do processo filho 2: " << getpriority(PRIO_PROCESS, getpid()) <<endl;
				carga(1000);
				led6.toggleValue();
				
			}

		}else{
			//processo pai
			

		    while(1){
		    	
		    	//cout << "Processo pai"<< endl;
		    	pot4Value = pot4.getPercentValue();
		    	pot6Value = pot6.getPercentValue();
		    	//filho 1
			    if(pot4Value > 56){
		    		setpriority(PRIO_PROCESS, pid1, 5);
			    }else{
			    	setpriority(PRIO_PROCESS, pid1, 19);
			    }

			    //filho 2

			    if(pot6Value > 56){
			    	setpriority(PRIO_PROCESS, pid2, 5);
			    }else{
			    	setpriority(PRIO_PROCESS, pid2, 19);
			    }
			    carga(1000);
		    }
		    
		}
	}
	
    return 0;
}
