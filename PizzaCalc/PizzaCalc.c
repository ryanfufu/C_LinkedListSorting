#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.14159265358979323846
struct pizza{
	char name[64] ;
	float size;
	float price;
	float pizzaperdollar;
	struct pizza* next;};

int main(int argc, const char* argv[]){
	int counter = 0;
	struct pizza *head;
	head = (struct pizza*)malloc(sizeof(struct pizza));
	
	FILE *fp;
	/*check if file exists*/

	
	fp = fopen(argv[1], "r");
	if (fp!=NULL) {
   	 	fseek (fp, 0, SEEK_END);
   	 	int size = ftell(fp);
  	  if (size==0) {
     	   printf("PIZZA FILE IS EMPTY\n");
	   fclose(fp);
	   free(head);
	   return EXIT_SUCCESS;
   	 }else{
	 	fseek(fp, 0, SEEK_SET);
	}}
        
	

	fscanf(fp, "%s", &head->name);
		
	/*special case when DONE is the only word in file*/
	if (strcmp(head->name,"DONE")==0){
		printf("Done");
		return EXIT_SUCCESS;
	}
	fscanf(fp, "%f", &head->size);
	fscanf(fp, "%f", &head->price);
	head->pizzaperdollar = (powf((head->size)/2.0, 2.0) *PI)/(head->price);	
	/*creation of first head node*/
	
	
	struct pizza  *ptr;
	ptr = (struct pizza*) malloc(sizeof(struct pizza));
	head->next = ptr;
	while(fscanf(fp, "%s", &ptr->name)){
		if (strcmp(ptr->name,"DONE")==0){
			break;
		}
		
		fscanf(fp, "%f", &ptr->size);
		fscanf(fp, "%f", &ptr->price);
		if(ptr->price!=0){
		ptr->pizzaperdollar = (powf((ptr->size)/2.0, 2.0) *PI)/(ptr->price);}
		else{
			ptr->pizzaperdollar = 0;
		}
		ptr->next = (pizza*) malloc(sizeof(pizza));
		ptr = ptr->next;	
			
	}	
	ptr->next = NULL;


	fclose(fp);
	
	/*checking values of linked list*/
	/*deletes the last DONE node*/
	
		struct pizza* iterato = head;
		struct pizza* t;
		
	while(iterato->next !=NULL){
		t = iterato;
		iterato = iterato->next;
	}
	free(t->next);
	t->next = NULL;
	
	/* bubble sorting */
	int done = 0;
	while (!done){
		struct pizza *current = head;
		struct pizza *future = current->next;
		struct pizza *previous = NULL;
		
		done= 1;
		
		while(future!=NULL){
			if (current->pizzaperdollar < future->pizzaperdollar){
				if (current == head){
					head = future;
				}else{
					previous->next = future;
				}
				current->next = future->next;
				future->next = current;
				
				previous = future;
				future = current->next;
				done = 0;
			}else if(current->pizzaperdollar== future->pizzaperdollar){
				/*checking name if pizza prices are the same*/
				if (strcmp(current->name, future->name)>0){
					if (current == head){
						head = future;
					}else{
						previous->next = future;
					}
				current->next = future->next;
				future->next = current;
				previous = future;
				future = current->next;
				done = 0;
					}
					
					
			else{
				previous = current;
				current = current->next;
				future = current->next;

				}}
						
			else{
				previous = current;
				current = current->next;
				future = current->next;
			}
		}
		
	}
	
	/*traversing through linked list and printing values*/
		struct pizza* iterator = head;
		

	while(iterator->next !=NULL){
		if (strcmp(iterator->name,"DONE")==0){
		break;
		}
		printf("%s %f\n", iterator->name, iterator->pizzaperdollar);
		iterator = iterator->next;
	}
	printf("%s %f\n", iterator->name, iterator->pizzaperdollar);	
	
	
	while (head->next!=NULL){
		struct pizza* temp = head;
		head = head->next;
		free(temp);
	}
	
	/*	original
	while(iterator!=NULL){
		if (strcmp(iterator->name,"DONE")==0){
		break;
		}
		printf("%s %f\n", iterator->name, iterator->pizzaperdollar);
		free(iterator);
		iterator = iterator->next;	
	}*/
	
	free(iterator);
	/*free(ptr);*/
	return EXIT_SUCCESS;

}
