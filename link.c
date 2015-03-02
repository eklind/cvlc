// Implementation of sending and receiving data
#include "bstrlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
//extern bstring bfromcstr (const char * str);



int send_bit(char a);

void goHigh();
void goLow();

int main(){
	char b[] ="1";
	//bstring b;
	//b = bfromcstr("hello");
	//int l = blength(b);
	//printf("%d\n",l);
	int send_ok=send_bit(*b);
	if (send_ok){
		printf("%d\n",send_ok);
	}
return 0;

}//Main



int send_bit(char a){
	FILE *export_file = NULL;        //declare pointers
        FILE *IO_direction = NULL;
 	char str2[] = "out";
        char str[] = "60";
 	export_file = fopen ("/sys/class/gpio/export", "w");
        fwrite (str,sizeof(str),1, export_file);
        fclose (export_file);
        IO_direction = fopen ("/sys/class/gpio/gpio60/direction", "w");    // opens the "direction-file"
	fwrite (str2,sizeof(str2),1,IO_direction); // Sets the pin to work as output	
	fclose (IO_direction);
		printf("%s\n","Sending:");
		printf("%c\n",a);
		if(a  == '1'){
			goHigh();
		}
		else if(a == '0'){
			goLow();
		}
		else{
			printf("%s\n","Error:Unexpected data format, expects 0 or 1.");
			return -1;
		}

	printf("%s\n","Done with send, time to clean up");
	usleep(1000000);

	IO_direction = fopen ("/sys/class/gpio/gpio60/direction", "w");
	fwrite (str2,sizeof(str2),1,IO_direction);
	fclose(IO_direction);
        export_file = fopen ("/sys/class/gpio/unexport", "w");   //remove the mapping
        fwrite (str,sizeof(str),1,export_file);
        fclose (export_file);
	printf("%s\n","All files closed");
	return 0;
} //End of send bit

/*
int freqToDelay(int freq){
	//takes a frequenzy in hertz and returns a delay value in micro seconds
	int delay=1000000/freq;
	return delay;
}
*/
void goHigh(){
	//Sets the pin high
	char high_val[1] ="1";
	FILE *value_file = NULL;
	value_file = fopen("/sys/class/gpio/gpio60/value","w");
	fwrite(high_val,sizeof(high_val),1,value_file);
	fclose(value_file);
}

void goLow(){
	//set the pin low
	char low_val[1] = "0";
	FILE *value_file2 = NULL;
	value_file2 = fopen("/sys/class/gpio/gpio60/value","w");
	fwrite(low_val,sizeof(low_val),1,value_file2);
	fclose(value_file2);
}

/*

void cleanUp(){
	//Closes directories 
	FILE *exporter = NULL;
	char pin[] = "60";
	exporter = fopen("sys/class/gpio/unexport","w");
	fwrite(pin,sizeof(pin),1,exporter);
	fclose(exporter);
	printf("%s\n","Files Closed");
}
		
*/





