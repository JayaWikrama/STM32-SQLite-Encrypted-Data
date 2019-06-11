#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


void HEXtoDec(char hex[100], char cplt[100]);
void serialOpenPort();
void mySerialRead(char INP[100]);
void _encript(char _text[100], char _pass[10], int step);
void _decript(char _text[100], char _pass[10], int step);
int callback(void *, int, char **, char **);
int callbackNO(void *, int, char **, char **);
int callbackPL(void *, int, char **, char **);
void readSQL(int mode);
void *writeData();
void *realtimePlot();

char adc_val[5], plotTxt[20];
int maxID=0;
int fd;
int val=0;
int _menu=0;

int main(){
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
	/*OPTION TO EDIT or DELETE
	sqlite3 test.db "UPDATE Mytime SET Name  = 'hahahahaha' WHERE ID = 419"
	sqlite3 test.db 'DELETE FROM Mytime WHERE Name LIKE %:21:%'
	sqlite3 test.db 'DELETE FROM Mytime WHERE ID = 448'
	*/
}

void serialOpenPort(){
	if ((fd = serialOpen("/dev/ttyACM0", 115200))<0){
		printf("error");
	}
}

void mySerialRead(char INP[25]){
	int i=0;
	char kr;
	for(int j=0; j<22; j++) INP[j]='\0';
	kr=0;
	while(kr!='\n'){
		kr=0;
		kr = serialGetchar(fd);
		if(kr>=32 && kr<128){
			INP[i]=kr;
			i++;
		}
	}
	printf("%d", i);
	if (strlen(INP)>1)printf("%s ", INP);
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
			strcat(myOut, "' >> /home/pi/myPlot.txt");
			
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
    serialOpenPort();

	while(_menu==1){
	mySerialRead(allInp);
	HEXtoDec(allInp, allOut);
	_encript(allOut, "Jaya1234", 3);
	_encript(adc_val, "Jaya1234", 3);
	strcpy(allInp, "'");
	strcat(allInp, allOut);
	strcat(allInp, "', '");
	strcat(allInp, adc_val);
	strcat(allInp, "'");
	if(strlen(allInp)>15 && strlen(allInp)<25){
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
					strcat(plotTxt, "' >> myPlot.txt");
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
	system("gnuplot rtscript");
}
