#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>


int main()
{
	int children=0;
	int grandchildren=0;
	pid_t pid, pid2;
	printf("parent pid = %d\n",getpid());
	while(children<4 && (pid=fork())!=-1){
		if(pid==0){
			//child[2] has its own 7 children
			if(children==2){
				while(grandchildren<7 && (pid2=fork())!=-1 ){
					if(pid2==0){
						//let the granchild stay  in the memory
						do{sleep(1000);}while(1);
					}
					else{
						printf("creat process pid_grandchildren[%d] = %d\n",grandchildren,pid2);
						grandchildren++;
					}
				}
			}
			//let the child stay in the memory
			do{sleep(1000);}while(1);
		}//end child
		else{
			printf("creat process pid_children[%d] = %d\n",children,pid);
			children++;
		}
	}//end while
	
	printf("Bang!~~~~~~!!! something failed~~~~~~\n");
	do{sleep(1000);}while(1);
	
	return 0;

}
