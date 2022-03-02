#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

#define ifa_broadaddr ifa_ifu.ifu_broadaddr

typedef struct
{
	char name[16];
	char state[5];
	char ipv4[16];
	char ipv6[46];
	char netmask[16];
	char gateway[16];
	char broadcast[16];
} interface;



//gets total number of ifs

int number_of_ifs(struct ifaddrs *ifa, struct ifaddrs *ifaddr){

	int ifs = 0;

	for (ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		ifs++;	
	}

	return ifs;

}

//creates array of all interface names

char **names_of_ifs(struct ifaddrs *ifa, struct ifaddrs *ifaddr, int ifs){

	char **ifnames = malloc(sizeof(char*) * ifs);
	for(int i = 0; i < ifs; i++){
		ifnames[i] = malloc(sizeof(char) * 16);
	}

	int n = 0;
	for (ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		snprintf(ifnames[n], 16, "%s", ifa->ifa_name);
		n++;
	}

	return ifnames;
}

//gets index of unique interfaces and total number

int *unique_ifs(char **ifnames, int ifs, int *unique){

	int unq = 0;
	int *unqifs = malloc(sizeof(int) * ifs);
	int j;
	
	for (int i = 0; i < ifs; i++){
		for (j = 0; j < ifs; j++){
			if(!strcmp(ifnames[i], ifnames[j])){
				break;
			}
		}
			if (i == j){
				unqifs[i] = 1;
				unq++;
			}
	}


	*unique = unq;

	return unqifs;
}




void get_addresses(interface *interfaces, struct ifaddrs *ifa, struct ifaddrs *ifaddr, int ifs, int unq){

	ifa = ifaddr;

	for (int i = 0; i < ifs; i++){

		struct sockaddr_in *sa;

		sa = (struct sockaddr_in *) ifa->ifa_addr;
		
		if (sa != NULL && ifa->ifa_addr->sa_family ==AF_INET){

			char *addr = inet_ntoa(sa->sin_addr);
			for (int j = 0; j < unq; j++){

				if (!strcmp(interfaces[j].name, ifa->ifa_name)){
					snprintf(interfaces[j].ipv4, 16, "%s", addr);
					break;
				}
				

			}
		}
		ifa = ifa->ifa_next;
	}
}


void get_netmasks(interface *interfaces, struct ifaddrs *ifa, struct ifaddrs *ifaddr, int ifs, int unq){

	ifa = ifaddr;

	for (int i = 0; i < ifs; i++){

		struct sockaddr_in *sa;
		sa = (struct sockaddr_in *) ifa->ifa_netmask;
		
		if (sa != NULL && ifa->ifa_netmask->sa_family == AF_INET){

			char *addr = inet_ntoa(sa->sin_addr);
			for (int j = 0; j < unq; j++){

				if (!strcmp(interfaces[j].name, ifa->ifa_name)){
					snprintf(interfaces[j].netmask, 16, "%s", addr);
					break;
				}
				

			}
		}
		ifa = ifa->ifa_next;
	}
}



void get_b_addresses(interface *interfaces, struct ifaddrs *ifa, struct ifaddrs *ifaddr, int ifs, int unq){


		ifa = ifaddr;

	for (int i = 0; i < ifs; i++){

		struct sockaddr_in *sa;

		sa = (struct sockaddr_in *) ifa->ifa_broadaddr;
		
		if (sa != NULL && ifa->ifa_broadaddr->sa_family == AF_INET){

			char *addr = inet_ntoa(sa->sin_addr);
			for (int j = 0; j < unq; j++){

				if (!strcmp(interfaces[j].name, ifa->ifa_name)){
					snprintf(interfaces[j].broadcast, 16, "%s", addr);
					break;
				}
				

			}
		}
		ifa = ifa->ifa_next;
	}
}


//sets interface names

void set_if_name(interface *interfaces, char **ifnames, int *unq_ifs_index, int unq){

	for(int i = 0; i < unq; i++){
		snprintf(interfaces[i].name, 16, "%s", ifnames[i]);
	}

}


//set interface states

void get_states(interface *interfaces, struct ifaddrs *ifa, struct ifaddrs *ifaddr, char **ifnames, int ifs, int unq){

	ifa = ifaddr;
	int *up_index = malloc(sizeof(int) * ifs);
	
	for (int i = 0; i < ifs; i++) {
		
		if((ifa->ifa_flags & IFF_RUNNING)){
			up_index[i] = 1;
		}

		else{
			up_index[i] = 0;
		}

		ifa = ifa->ifa_next;
	}


	int up, name;
	for (int i = 0; i < unq; i++){
		up = up_index[i];

		for (int j = 0; j < ifs; j++){

			//compare names
			name = (!strcmp(interfaces[i].name, ifnames[j]));
			
			if (name && up){
				snprintf(interfaces[i].state, 5, "UP");
				break;
			}

			if (name && !up){
				if(!strcmp(interfaces->state, "UP"))
				snprintf(interfaces[i].state, 5, "DOWN");
				continue;
			}
		}
	}
	free(up_index);
}


void print_ifs(interface *interfaces, int unq){
	
	printf("\nIP Configuration\n");

	for (int i = 0; i < unq; i++){
		printf("\n%s:\n\n", interfaces[i].name);
		printf("\tState:        \t\t%s\n", interfaces[i].state);

		if (strlen(interfaces[i].ipv4) != 0){
			printf("\tIPv4 Address: \t\t%s\n", interfaces[i].ipv4);
		}
		else{
			printf("\tIPv4 Address: \t\t. . . . . . . . \n");
		}

		if (strlen(interfaces[i].broadcast) != 0){
			printf("\tBRD  Address: \t\t%s\n", interfaces[i].broadcast);
		}
		else{
			printf("\tBRD  Address: \t\t. . . . . . . . \n");
		}

		if (strlen(interfaces[i].netmask) != 0){
			printf("\tNetmask:      \t\t%s\n", interfaces[i].netmask);
		}		
		else{
			printf("\tNetmask:      \t\t. . . . . . . . \n");
		}
	}
	printf("\n");


}



int main(){

	struct ifaddrs *ifa, *ifaddr;
	struct sockaddr_in *sa;
	char *addr, *mask, *broad;
	int unq = 0;

	//create linked list of interfaces
	if (getifaddrs(&ifaddr) == -1){
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	//gets number of interfaces
	int ifs = number_of_ifs(ifa, ifaddr);

	//create array of interface names	
	char **ifnames = names_of_ifs(ifa, ifaddr, ifs);

	//find unique interfaces
	int *unq_ifs_index = unique_ifs(ifnames, ifs, &unq);

	interface *interfaces = calloc(unq, sizeof(interface));

	set_if_name(interfaces, ifnames, unq_ifs_index, unq);
	get_states(interfaces, ifa, ifaddr, ifnames, ifs, unq);
	get_addresses(interfaces, ifa, ifaddr, ifs, unq);
	get_b_addresses(interfaces, ifa, ifaddr, ifs, unq);
	get_netmasks(interfaces, ifa, ifaddr, ifs, unq);

	print_ifs(interfaces, unq);

	//free memory

	for(int i = 0; i < ifs; i++){
			free(ifnames[i]);
		}
	
	free(ifnames);
	free(unq_ifs_index);
	free(interfaces);
	freeifaddrs(ifaddr);

	return 0;
}
