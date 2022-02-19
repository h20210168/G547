#include <stdio.h> 
#include<stdlib.h>
#include<fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include<sys/ioctl.h>

#define MAGIC_NUM 900
#define SET_CHANNEL _IOW(MAGIC_NUM,0,int32_t*)
#define SET_ALLIGNMENT _IOW(MAGIC_NUM,1,int32_t*)
#define SET_MODE _IOW(MAGIC_NUM,2,int32_t*)


int main()
{ 
  while(1)
  {
  int a;
  int fd;
  uint16_t data;
  uint32_t channel,mode,allignment;
  fd=open( "/dev/adc-dev", O_RDWR);
  
  if(fd  == -1)
  {
  
    printf("ADC file cannot be read\n");
    return 0;
  }

    printf("Please select ADC Channel No. (0-7):-\n");
    scanf("%d",&channel);
     if (channel>= 8 || channel<0 )
    {
    printf("WARNING : Please enter valid ADC Channel No.\n");
    break;
    }
    else
    {
    ioctl(fd,SET_CHANNEL, (int32_t*) &channel); 
    }

    printf("Please select the desired allignment:  (0: Right alligned, 1: Left alligned):-\n");
    scanf("%d",&allignment);
    if (allignment >= 2 || allignment <0)
    {
    printf("WARNING : Please enter proper allignment\n");
    break;
    }
    else
    {
    ioctl(fd, SET_ALLIGNMENT , (int32_t*) &allignment);
    } 
    
    printf("Please select the desired conversion mode:  (0:single shot conversion mode, 1: continuous conversion mode):-\n");
    scanf("%d",&mode);
    if (mode >= 2 || mode< 0 )
    {
    printf("WARNING : Please enter proper conversion mode\n");
    break;
    }
    else
    {
    ioctl(fd, SET_MODE , (int32_t*) &mode);
    } 
  
  read(fd,&data,2);
  printf("ADC data %x\n",data);

  printf("Do you want another reading? (Yes-1, No-0): \n");
  scanf("%d",&a);

  if (a==1)
  {
  continue;
  }
  else if(a==0)
  {
    break;
  }
  else if (a!=0 || a!=1)
  {
    printf("Enter valid response");
    break;
  }

  close(fd);
 } 
  return 0;
}


