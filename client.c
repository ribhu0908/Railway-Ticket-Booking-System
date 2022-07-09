/*
MT2021106
Ribhu Mukherjee
Client side:
socket->connect->send/recv->close
*/

#include <string.h>
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h> 
	 
#define PORT 8000

//------------------------------Function declarations---------------------------------------//

int client(int sock);
int menu(int sock,int type);
int user_function(int sock,int choice);
int crud_train(int sock,int choice);
int crud_user(int sock,int choice);

//------------------------------MAIN PROGRAM--------------------------------------------//
int main(void) { 
	int sock; 
    	struct sockaddr_in server; 
    	char server_reply[50],*server_ip;
	server_ip = "127.0.0.1"; //loop back address
        
        //client socket descriptor is made
    	sock = socket(AF_INET, SOCK_STREAM, 0); 
    	if (sock == -1) { 
       	printf("Could not create socket"); 
    	} 
        
        //put server ip address in sockaddr_in(server)
    	server.sin_addr.s_addr = inet_addr(server_ip); 
    	server.sin_family = AF_INET; 
    	server.sin_port = htons(PORT); 
        
        //client sends a connection request to server
    	if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
       	perror("connect failed. Error"); 
    	
    	//run loop till user gives 3 i.e. exit
	while(client(sock)!=3);
	
    	close(sock); //close client socket when user wants to exit(gives 3)
	return 0; 
} 

//-------------------- First function which is called-----------------------------//

int client(int sock){
	int choice,valid;
	//system("clear");
	printf("\n\n\t\t\tWELCOME TO TRAIN RESERVATION SYSTEM!!!!!!\n\n");
	printf("\t1. Log In\n");
	printf("\t2. Sign Up\n");
	printf("\t3. Exit\n");
	printf("\tEnter Your Choice:: ");
	
	//put choice in sock_fd for server to know which statements of above 3
	scanf("%d", &choice);
	write(sock, &choice, sizeof(choice));
	if (choice == 1){					// Log in
		int id,type;
		char password[50];
		printf("\tlogin id:: ");
		scanf("%d", &id);
		//Prompt the user for a password without echoing
		strcpy(password,getpass("\tpassword:: "));
		
	        //put id and password for type of user specified above
		write(sock, &id, sizeof(id));
		write(sock, &password, sizeof(password));
		
		read(sock, &valid, sizeof(valid));
		if(valid){
			printf("\tlogin successfully\n");
			
			//read the type of user[regular,agent,admin] after successful login
			read(sock,&type,sizeof(type));
			while(menu(sock,type)!=-1);
			//system("clear");
			return 1;
		}
		else{
			printf("\tLogin Failed : Incorrect password or login id\n");
			return 1;
		}
	}
	
	//SIGN UP OPTION
	else if(choice == 2){					
		int type,id;
		char name[50],password[50],secret_pin[6];
		printf("\n\tEnter The Type Of Account:: \n");
		printf("\t0. Admin\n\t1. Agent\n\t2. Customer\n");
		printf("\tYour Response: ");
		scanf("%d", &type);
		printf("\tEnter Your User Name: ");
		scanf("%s", name);
		strcpy(password,getpass("\tEnter Your Password: "));
		
		//The user is entering as admin[if block only for admin]
		if(!type){
			//while loop runs till admin gives the right secret pin
			while(1){
			        //secret key is needed to sign up as an admin
				strcpy(secret_pin, getpass("\tEnter the secret PIN to create ADMIN account(ADMIN SAFETY): "));
				if(strcmp(secret_pin, "secret")!=0) 					
					printf("\tInvalid PIN. Please Try Again.\n");
				else
					break;
			}
		}
		
		//write type, name and passowrd to sock_fd
		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &password, strlen(password));
		
		//recieve id from server
		read(sock, &id, sizeof(id));
		printf("\tRemember Your Unique login id For Further Logins as :: %d\n", id);
		return 2;
	}
	else
		//EXIT 							
		return 3;
	
}

//-------------------- Main menu function-----------------------------//

//type---> Regular/agent and admin
int menu(int sock,int type){
	int choice;
	//AGENT AND CUSTOMER
	if(type==2 || type==1){					
		printf("\t1. Book Ticket\n");
		printf("\t2. View your Bookings\n");
		printf("\t3. Update your Booking\n");
		printf("\t4. Delete booking\n");
		printf("\t5. Logout\n");
		printf("\tYour Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		return user_function(sock,choice);
	}
	//ADMIN BLOCK
	else if(type==0){					
		printf("\n\t1. ADMIN: TRAIN CRUD operations\n");
		printf("\t2. ADMIN: USER CRUD operations\n");
		printf("\t3. ADMIN: Logout\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
			//train crud operations
			if(choice==1){
				printf("\t1. Add train\n");
				printf("\t2. View train details\n");
				printf("\t3. Modify train\n");
				printf("\t4. Delete train\n");
				printf("\t Your Choice: ");
				scanf("%d",&choice);	
				write(sock,&choice,sizeof(choice));
				return crud_train(sock,choice);
			}
			//user crud operations
			else if(choice==2){
				printf("\t1. Add User\n");
				printf("\t2. View all users\n");
				printf("\t3. Modify user\n");
				printf("\t4. Delete user\n");
				printf("\t Your Choice: ");
				scanf("%d",&choice);
				write(sock,&choice,sizeof(choice));
				return crud_user(sock,choice);
			
			}
			else if(choice==3)
				return -1;
	}	
	
}

//-------------------------------- crud operations on train-----------------------------//
int crud_train(int sock,int choice){
	int valid = 0;
	if(choice==1){				// Add train response
		char tname[50];
		printf("\n\tEnter train name: ");
		scanf("%s",tname);
		write(sock, &tname, sizeof(tname));
		
		//read response from server(valid or not)[CRUD OPERATION IS VALID OR NOT]
		//is the addition successful??, is returned by the valid variable from server
		read(sock,&valid,sizeof(valid));	
		if(valid)
			printf("\n\tTrain added successfully\n");

		return valid;	
	}
	
	else if(choice==2){			// View train response
		int no_of_trains;
		int tno;
		char tname[50];
		int tseats;
		int aseats;
		read(sock,&no_of_trains,sizeof(no_of_trains));

		printf("\tT_no\tT_name\tT_seats\tAvail_seats\n");
		while(no_of_trains--){
			read(sock,&tno,sizeof(tno));
			read(sock,&tname,sizeof(tname));
			read(sock,&tseats,sizeof(tseats));
			read(sock,&aseats,sizeof(aseats));
			
			//if train name is "deleted", dont print the associated train attributes
			if(strcmp(tname, "deleted")!=0)
				printf("\t%d\t%s\t%d\t%d\n",tno,tname,tseats,aseats);
		}

		return valid;	
	}
	
	else if (choice==3){			// Update train response
		int tseats,choice=2,valid=0,tid;
		char tname[50];
		
		write(sock,&choice,sizeof(int));
		crud_train(sock,choice); //displaying train detail
		
		//if you want to update train details, enter the train ID
		printf("\n\t Enter the train number you want to modify: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
		
		//we cant change the train ID, as an admin we can only change train name and total seats
		printf("\n\t1. Train Name\n\t2. Total Seats\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		
		if(choice==1){
			read(sock,&tname,sizeof(tname));
			printf("\n\t Current name: %s",tname);
			printf("\n\t Updated name:");
			scanf("%s",tname);
			write(sock,&tname,sizeof(tname));
		}
		else if(choice==2){
			read(sock,&tseats,sizeof(tseats));
			printf("\n\t Current value: %d",tseats);
			printf("\n\t Updated value:");
			scanf("%d",&tseats);
			write(sock,&tseats,sizeof(tseats));
		}
		//is the updation successful??, is returned by the valid variable from server
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t Train data updated successfully\n");
		return valid;
	}

	else if(choice==4){				// Delete train response
		int choice=2,tid,valid=0;
		write(sock,&choice,sizeof(int));
		crud_train(sock,choice); //displaying train details
		
		printf("\n\t Enter the train number you want to delete: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
		//is the deletion successful??, is returned by the valid variable from server
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t Train deleted successfully\n");
		return valid;
	}
	
}

//-------------------------------- crud operations on user-----------------------------//
//can be done by ADMIN

int crud_user(int sock,int choice){
	int valid = 0;
	//admin can add customer/agent
	if(choice==1){							// Add user
		int type,id;
		char name[50],password[50];
		printf("\n\tEnter The Type Of Account, you want to add!!:: \n");
		printf("\t1. Agent\n\t2. Customer\n");
		printf("\tYour Response: ");
		scanf("%d", &type);
		printf("\tUser Name: ");
		scanf("%s", name);
		strcpy(password,getpass("\tPassword: "));
		
		
		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &password, strlen(password));
		
		//is the addition successful?? is returned by valid
		read(sock,&valid,sizeof(valid));
		
		//if valid, then server returns the ID of the added user	
		if(valid){
			read(sock,&id,sizeof(id));
			printf("\tRemember Your login id For Further Logins as :: %d\n", id);
		}
		return valid;	
	}
	
	else if(choice==2){						// View user list
		int no_of_users;
		int id,type;
		char uname[50];
		read(sock,&no_of_users,sizeof(no_of_users));

		printf("\tU_id\tU_name\tU_type\n");
		while(no_of_users--){
			read(sock,&id,sizeof(id));
			read(sock,&uname,sizeof(uname));
			read(sock,&type,sizeof(type));
			
			//if we dont get username as "deleted", display it
			if(strcmp(uname, "deleted")!=0)
				printf("\t%d\t%s\t%d\n",id,uname,type);
		}

		return valid;	
	}

	else if (choice==3){						// Update user
		int choice=2,valid=0,uid;
		char name[50],pass[50];
		
		write(sock,&choice,sizeof(int));
		crud_user(sock,choice); //display user details
		
		printf("\n\t Enter the U_id you want to modify: ");
		scanf("%d",&uid);
		write(sock,&uid,sizeof(uid)); //put userid into sock and send to server
		
		//admin can change only users's username and password
		printf("\n\t1. User Name\n\t2. Password\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));
		
		//changing username
		if(choice==1){
			read(sock,&name,sizeof(name));
			printf("\n\t Current name: %s",name);
			printf("\n\t Updated name:");
			scanf("%s",name);
			write(sock,&name,sizeof(name));
			read(sock,&valid,sizeof(valid));
		}
		else if(choice==2){
			//old password firstly is being checked, and after successful checking, you can only enter new password
			//if old password is wrong, you cannot enter new password
			printf("\n\t Enter Current password: ");
			scanf("%s",pass);
			write(sock,&pass,sizeof(pass));
			read(sock,&valid,sizeof(valid));
			if(valid){
				printf("\n\t Enter new password:");
				scanf("%s",pass);
			}
			else
				printf("\n\tIncorrect password\n");
			
			write(sock,&pass,sizeof(pass)); //old password in case of incorrect login, new password in case of correct login
		}
		if(valid){
			read(sock,&valid,sizeof(valid));
			if(valid)
				printf("\n\t User data updated successfully\n");
		}
		return valid;
	}

	else if(choice==4){						// Delete a user
		int choice=2,uid,valid=0;
		write(sock,&choice,sizeof(int));
		crud_user(sock,choice); //display user list
		
		printf("\n\t Enter the id you want to delete: ");
		scanf("%d",&uid);
		write(sock,&uid,sizeof(uid));
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\t User deleted successfully\n");
		return valid;
	}
}

//-------------------------------- User function to book tickets -----------------------------//
//FUNCTIONALITIES PERFORMED BY CUSTOMER/AGENT
int user_function(int sock,int choice){
	int valid =0;
	if(choice==1){										// Book tickets
		int view=2,tid,seats;
		write(sock,&view,sizeof(int));
		crud_train(sock,view); //display all train details
		printf("\n\tEnter the train number you want to book: ");
		scanf("%d",&tid);
		write(sock,&tid,sizeof(tid));
				
		printf("\n\tEnter the no. of seats you want to book: ");
		scanf("%d",&seats);
		write(sock,&seats,sizeof(seats));
	
		read(sock,&valid,sizeof(valid));
		//if successfull booking, valid is non zero
		if(valid)
			printf("\n\tTicket booked successfully.\n");
		else
			printf("\n\tSeats were not available.\n");

		return valid;
	}
	
	else if(choice==2){									// View the bookings
		int no_of_bookings;
		int id,tid,seats;
		read(sock,&no_of_bookings,sizeof(no_of_bookings));

		printf("\tB_id\tT_no\tSeats\n");
		while(no_of_bookings--){
			read(sock,&id,sizeof(id));
			read(sock,&tid,sizeof(tid));
			read(sock,&seats,sizeof(seats));
			
			//prints the bookings per user
			//if user seats = 0, dont display it
			if(seats!=0)
				printf("\t%d\t%d\t%d\n",id,tid,seats);
		}

		return valid;
	}

	else if(choice==3){									// Update a booking (increment/ decrement seats)
		int choice = 2,bid,val,valid;
		user_function(sock,choice); //print all bookings
		printf("\n\t Enter the B_id you want to modify: ");
		scanf("%d",&bid);
		write(sock,&bid,sizeof(bid));

		printf("\n\t1. Increase number of seats\n\t2. Decrease number of seats\n");
		printf("\t Your Choice: ");
		scanf("%d",&choice);
		write(sock,&choice,sizeof(choice));

		if(choice==1){
			printf("\n\tNo. of tickets to increase");
			scanf("%d",&val);
			write(sock,&val,sizeof(val));
		}
		else if(choice==2){
			printf("\n\tNo. of tickets to decrease");
			scanf("%d",&val);
			write(sock,&val,sizeof(val));
		}
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\tBooking updated successfully.\n");
		else
			printf("\n\tUpdation failed. No more seats available.\n");
		return valid;
	}
	
	else if(choice==4){									// Cancel 1 booking
		int choice = 2,bid,valid;
		user_function(sock,choice); //view all bookings
		printf("\n\t Enter the B_id you want to cancel: ");
		scanf("%d",&bid);
		
		//The deletion logic is handled by the server
		write(sock,&bid,sizeof(bid));
		read(sock,&valid,sizeof(valid));
		if(valid)
			printf("\n\tBooking cancelled successfully.\n");
		else
			printf("\n\tCancellation failed.\n");
		return valid;
	}
	else if(choice==5)									// Logout
		return -1;
	
}
