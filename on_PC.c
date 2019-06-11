#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>        
#include <stdlib.h> 
#include <unistd.h>
#include <sqlite3.h>
#include <pthread.h>

#define UART_PORT "/dev/ttyACM1"
#define BAUDRATE B115200


void UART_OPEN_PORT(void);
void UART_RECEIVE(char INP[25]);
void HEXtoDec(char hex[100], char cplt[100]);
void _encript(char _text[100], char _pass[10], int step);
void _decript(char _text[100], char _pass[10], int step);
int callback(void *, int, char **, char **);
int callbackNO(void *, int, char **, char **);
int callbackPL(void *, int, char **, char **);
void readSQL(int mode);
void *writeData();
void *realtimePlot();

//UART VAR START
char adc_val[5], plotTxt[20];
int maxID=0;
int fd;
int val=0;
int _menu=0;

struct termios oldtp, newtp;
//UART VAR END


int  main()
{
        while(_menu != 1 && _menu !=2 && _menu!=3){
		system("echo aku");
		printf("::MENU::\n\t1. Record Data\n\t2. View Data\n\t3. View Plot\nYour Choice > ");
		scanf("%d", &_menu);
	} 
	if(_menu==1){
		pthread_t rThread;
		pthread_t wThread;

		pthread_create(&wThread, NULL, writeData, NULL);
		pthread_create(&rThread, NULL, realtimePlot, NULL);
		while(1) sleep(1);
	}
	else if(_menu==2) readSQL(1);
	else if(_menu==3){
		system("echo > myPlot.txt");
		readSQL(2);
		system ("gnuplot script");
	}
    return 0;
}

void  UART_RECEIVE(char INP[25])
{
	unsigned char buff;
    int read_cnt;
    for (read_cnt=0; read_cnt<25; read_cnt++) INP[read_cnt]=0;
    read_cnt = 0;
    while (read_cnt<20 && buff!='\n'){
        buff=0;
        read(fd, &buff, 1);
        INP[read_cnt]=buff;
        read_cnt++;
    }
    for(int i=0; i<10; i++) usleep(10000);
    INP[read_cnt-1]=0;
    if (strlen(INP)>=10 && strlen(INP)<=24)
        printf("%s\n", INP);
}

void UART_OPEN_PORT(void)
{
         
	fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY );
	printf("UART RECEIVE START : %d\n\n", fd);
    if (fd <0) perror(UART_PORT);
}


void HEXtoDec(char hex[100], char cplt[100]){
    int idx=0, idx_part=0, itrl=0;
    char hex_part[5], dec_part[5];
    strcpy(cplt, "");
    for(int i=0; i<5; i++) hex_part[i]=0;
    while(hex[idx]!='\n' && hex[idx]!='\0'){
        if(hex[idx]!='-'){
            hex_part[idx_part]=hex[idx];
            idx_part++;
        }
        else{
            sscanf(hex_part, "%x", &val);
            sprintf(hex_part, "%d", val);
            strcat(cplt,hex_part);
            for(int i=0; i<5; i++) hex_part[i]=0;
            idx_part=0;
            itrl++;
            if (itrl==3) strcat(cplt, " ");
            else if(itrl==6) strcat(cplt, "");
            else if (itrl<3) strcat(cplt, "-");
            else if (itrl>3 && itrl<6) strcat(cplt, ":");
        }
        idx++;
    }
    sscanf(hex_part, "%x", &val);
    sprintf(hex_part, "%d", val);
    strcpy(adc_val,hex_part);
}

void _encript(char _text[100], char _pass[10], int step){
    int txt_idx=0, pss_idx=0;
    char txt_tmp[100];
    strcpy(txt_tmp, "");
    while(txt_idx<strlen(_text)){
        if ((txt_idx%strlen(_pass))%step != 0){
            if(_text[txt_idx]>='a' && _text[txt_idx]<='z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - 'a'))%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)))%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)))%26 + 'a');
                else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else if(_text[txt_idx]>='A' && _text[txt_idx]<='Z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - 'a'))%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)))%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)))%26 + 'A');
                else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            _text[txt_idx]=txt_tmp[txt_idx];
            txt_idx++;
        }
        else{
            if(_text[txt_idx]>='a' && _text[txt_idx]<='z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - 'a') + 26)%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)) + 26)%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)) + 26)%26 + 'a');
                else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else if(_text[txt_idx]>='A' && _text[txt_idx]<='Z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - 'a') + 26)%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)) + 26)%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)) + 26)%26 + 'A');
                else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            _text[txt_idx]=txt_tmp[txt_idx];
            txt_idx++;
        }
    }
}

void _decript(char _text[100], char _pass[10], int step){
    int txt_idx=0, pss_idx=0;
    char txt_tmp[100];
    int _long=0;
    _long = strlen(_text);
    strcpy(txt_tmp, "");
    while(txt_idx<_long){
        if ((txt_idx%strlen(_pass))%step == 0){
            if(_text[txt_idx]>='a' && _text[txt_idx]<='z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - 'a'))%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)))%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') + (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)))%26 + 'a');
                else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else if(_text[txt_idx]>='A' && _text[txt_idx]<='Z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - 'a'))%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)))%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') + (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)))%26 + 'A');
                else txt_tmp[txt_idx] = _text[txt_idx] + (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            _text[txt_idx]=txt_tmp[txt_idx];
            txt_idx++;
        }
        else{
            if(_text[txt_idx]>='a' && _text[txt_idx]<='z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - 'a') + 26)%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)) + 26)%26 + 'a');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'a') - (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)) + 26)%26 + 'a');
                else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else if(_text[txt_idx]>='A' && _text[txt_idx]<='Z'){
                if (_pass[txt_idx%strlen(_pass)] >= 'a' && _pass[txt_idx%strlen(_pass)] <= 'z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - 'a') + 26)%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= 'A' && _pass[txt_idx%strlen(_pass)] <= 'Z') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - 'A' + txt_idx%strlen(_pass)) + 26)%26 + 'A');
                else if (_pass[txt_idx%strlen(_pass)] >= '0' && _pass[txt_idx%strlen(_pass)] <= '9') txt_tmp[txt_idx] = (((_text[txt_idx] - 'A') - (_pass[txt_idx%strlen(_pass)] - '0' + 2*txt_idx%strlen(_pass)) + 26)%26 + 'A');
                else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            }
            else txt_tmp[txt_idx] = _text[txt_idx] - (_pass[txt_idx%strlen(_pass)]%strlen(_pass));
            _text[txt_idx]=txt_tmp[txt_idx];
            txt_idx++;
        }
    }
}

int callbackPL(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    char myTxt[20], myOut[100];
    char decrp[20];
    int _id=0;
    
    for (int i = 0; i < argc; i++) {
		if (strcmp(azColName[i], "ID")==0){
			strcpy(myTxt, argv[i] ? argv[i] : "NULL");
			strcat(myTxt, " ");
			_id++;
		}
		else if ((strcmp(azColName[i], "Data")==0)){
			strcpy(decrp, argv[i] ? argv[i] : "NULL");
			_decript(decrp, "Jaya1234", 3);
			strcat(myTxt, decrp);
			strcpy(myOut, "echo '");
			strcat(myOut, myTxt);
			strcat(myOut, "' >> myPlot.txt");
			
			system(myOut);
		}
    }
    return 0;
}

int callbackNO(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
		if (strcmp(azColName[i], "ID")==0)sscanf(argv[i] ? argv[i] : "NULL", "%d", &maxID);
    }
    return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    char myTxt[100];
    
    for (int i = 0; i < argc; i++) {
		strcpy(myTxt, argv[i] ? argv[i] : "NULL");
		if (strcmp(azColName[i], "ID")!=0) _decript(myTxt, "Jaya1234", 3);
		else sscanf(myTxt, "%d", &maxID);
        printf("%s = %s\n", azColName[i], myTxt);
    }
    
    printf("\n");
    
    return 0;
}

void readSQL(int mode){
	sqlite3 *db;
    char *errMsg = 0;
 
	if(sqlite3_open("test.db", &db) != SQLITE_OK) printf("fail to open file. %s\n", sqlite3_errmsg(db));
	else{
		char *sql = "SELECT * FROM MyData";
		if (mode == 1) if(sqlite3_exec(db, sql, callback, 0, &errMsg) != SQLITE_OK) printf("fail to read file. error MSG : %s\n", errMsg);
		if (mode == 0) if(sqlite3_exec(db, sql, callbackNO, 0, &errMsg) != SQLITE_OK) printf("fail to read file. error MSG : %s\n", errMsg);
		if (mode == 2) if(sqlite3_exec(db, sql, callbackPL, 0, &errMsg) != SQLITE_OK) printf("fail to read file. error MSG : %s\n", errMsg);
	}
	sqlite3_close(db);
}

void *writeData(){
	char allInp[25], allOut[50], id_text[10];
    sqlite3 *db;
    char *errMsg = 0;
    int id=0;
    readSQL(0);
    id=maxID;
    UART_OPEN_PORT();

	while(_menu==1){
	UART_RECEIVE(allInp);
	HEXtoDec(allInp, allOut);
	_encript(allOut, "Jaya1234", 3);
	_encript(adc_val, "Jaya1234", 3);
	strcpy(allInp, "'");
	strcat(allInp, allOut);
	strcat(allInp, "', '");
	strcat(allInp, adc_val);
	strcat(allInp, "'");
	if(strlen(allInp)>15 && strlen(allInp)<35){
		if(sqlite3_open("test.db", &db) != SQLITE_OK) printf("ERR : %s\n", sqlite3_errmsg(db));
		else{
			char *sql;
			sql = "CREATE TABLE MyData(ID INT, Time TEXT, Data TEXT);";
			if(sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK){
				if(strcmp("table MyData already exists", errMsg)!=0)printf("(err) %s\n", errMsg);
				else {
					char sql2[200];
					id++;
					
					sprintf(id_text, "%d ", id);
					strcpy(plotTxt, "echo '");
					strcat(plotTxt, id_text);
					sprintf(adc_val, "%d",val);
					strcat(plotTxt, adc_val);
                    if (id%100==0) strcat(plotTxt, "' > myPlot.txt");
                    else strcat(plotTxt, "' >> myPlot.txt");
					system(plotTxt);
					
					sprintf(id_text, "%d, ", id);
					strcpy(sql2, "INSERT INTO MyData VALUES (");
					strcat(sql2, id_text);
					strcat(sql2, allInp);
					strcat(sql2, ");");
					if(sqlite3_exec(db, (char *) sql2, 0, 0, &errMsg) != SQLITE_OK) printf("(err) %s\n", errMsg);
					else if ((int)sqlite3_last_insert_rowid(db) == 1) printf("Succes to insert : %dst row\n", id);
					else if ((int)sqlite3_last_insert_rowid(db) == 2) printf("Succes to insert : %dnd row\n", id);
					else if ((int)sqlite3_last_insert_rowid(db) == 3)printf("Succes to insert : %drd row\n", id);
					else printf("Succes to insert : %dth row\n", id);
				}
			}
		}
		sqlite3_close(db);
	}
	}
}

void *realtimePlot(){
    char tengah[20], akhir[100], total[100];
    int span = 100;
    int min_range = 0;
    int id_temp = 0;
    FILE *myGraphSet;
        //if (maxID != 0){
            /*
            strcpy(total, "set title 'ADC Plot'\nset xrange [");
            min_range = maxID - span;
            sprintf(tengah, "%d", min_range);
            strcat(total, tengah);
            strcat(total, ":");
            sprintf(tengah, "%d", maxID+1);
            strcat(total, tengah);
            strcat(total, "]\nset xlabel 'time(s)'\nset ylabel 'adc(12b)'\nplot 'myPlot.txt' with line\npause 1");
            myGraphSet = fopen("rtscript.out", "w");
            fprintf(myGraphSet, "%s", total);
            fclose(myGraphSet);*/
	        system("gnuplot rtscript.out");
        sleep(1);
}
