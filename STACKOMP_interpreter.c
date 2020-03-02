#import <stdio.h>
#include <string.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

typedef enum {FREE,CHAR,NUM} celltype;
typedef enum {RIGHT,DOWN,LEFT,UP} direct;


struct  coord{
  int X;
  int Y;
};
struct cell{
  int value;
  celltype type;
};

struct skdata{
  int line_count;
  int num_columns;
  char cell_txt[100][1000];
  struct cell cell_dat[100][1000];
  // celltype cell_dat.type[100][1000];
  struct coord ID_POS;
  int MEM_VAL;
  bool STORING;
  celltype MEM_TYPE;
  direct dir;
  char output_store[6000];
} DATA;

//COORD FUNCTIONS
bool coord_equals(struct coord A,struct coord B){
  return (A.X==B.X)&&(A.Y==B.Y);
}
struct coord copy(struct coord tocopy){
  struct coord copyed;
  copyed.X=tocopy.X;
  copyed.Y=tocopy.Y;
  return copyed;
}

struct coord shift_coord(struct coord toshift,direct direction){
  switch(direction){
    case RIGHT:
      toshift.X++;
      break;
    case DOWN:
      toshift.Y++;
      break;
    case LEFT:
      toshift.X--;
      break;
    case UP:
      toshift.Y--;
      break;
  }
  return toshift;
}
struct coord loop_coord(struct coord toloop){
  //LOOPING AROUND IN X AN Y
  if(toloop.X>DATA.num_columns){
    toloop.X=0;
  }
  if(DATA.cell_dat[toloop.Y][toloop.X].value=='\n'&&(DATA.cell_dat[toloop.Y][toloop.X].type== FREE)){
    toloop.X=0;
  }
  if(toloop.X<0){//LOOP AROUND X
    while((DATA.cell_dat[toloop.Y][toloop.X].type!= FREE)||(DATA.cell_dat[toloop.Y][toloop.X].value!='\n')){
      toloop.X++;
    }
    toloop.X--;
  }
  toloop.Y= (toloop.Y+DATA.line_count)%DATA.line_count;// LOOP AROUND Y
  return toloop;
}

//CELL DATA FUNCTIONS

void store_cell_data(struct coord cellat){
  DATA.MEM_VAL=DATA.cell_dat[cellat.Y][cellat.X].value;
  DATA.MEM_TYPE=DATA.cell_dat[cellat.Y][cellat.X].type;
}
void copy_cell_data(struct coord cellat,struct coord cellto){
  DATA.cell_dat[cellto.Y][cellto.X].value=DATA.cell_dat[cellat.Y][cellat.X].value;
  DATA.cell_dat[cellto.Y][cellto.X].type=DATA.cell_dat[cellat.Y][cellat.X].type;
}



#define ask_for_filename true

void render(){
  printf("  ");
  for(int j=0; j<DATA.num_columns;j++){
    printf("_");
  }
  printf("\n");
  for(int i=0;i<DATA.line_count;i++){
    printf("x| ");
    for(int j=0; j<DATA.num_columns;j++){
      if(i==DATA.ID_POS.Y && j==DATA.ID_POS.X){
        printf("&");
      }else if(DATA.cell_dat[i][j].type==NUM){
        printf("*");
      }else if(DATA.cell_dat[i][j].value=='\n'){
        printf("|x");
      }else{
        printf("%c",DATA.cell_dat[i][j].value);
      }
    }
    printf("\n");
  }
  printf("  ");
  for(int j=0; j<DATA.num_columns;j++){
    printf("_");
  }
  printf("\n");
}
void popStack(direct dir){
  struct coord shift_pos= copy(DATA.ID_POS);
  shift_pos= shift_coord(shift_pos,dir);
  store_cell_data(shift_pos);
  do{
    struct coord next_pos= copy(shift_pos);
    next_pos= shift_coord(shift_pos,dir);
    // next_pos.Y= (next_pos.Y+DATA.line_count)%DATA.line_count;
    next_pos= loop_coord(next_pos);
    copy_cell_data(next_pos,shift_pos);
    shift_pos=next_pos;
  }while((!coord_equals(shift_pos,DATA.ID_POS))&&(DATA.cell_dat[shift_pos.Y][shift_pos.X].type!=FREE));
}
void pushStack(direct dir){
  struct coord shift_pos= copy(DATA.ID_POS);
  shift_pos= shift_coord(shift_pos,dir);
  shift_pos=loop_coord(shift_pos);
  struct cell cell_store;
  cell_store.value=DATA.cell_dat[shift_pos.Y][shift_pos.X].value;
  cell_store.type=DATA.cell_dat[shift_pos.Y][shift_pos.X].type;

  if((DATA.MEM_TYPE==NUM)&&(DATA.cell_dat[shift_pos.Y][shift_pos.X].type==NUM)){
    if(DATA.MEM_VAL<DATA.cell_dat[shift_pos.Y][shift_pos.X].value){
      DATA.cell_dat[shift_pos.Y][shift_pos.X].value=DATA.cell_dat[shift_pos.Y][shift_pos.X].value-DATA.MEM_VAL;
      DATA.cell_dat[shift_pos.Y][shift_pos.X].type=NUM;
      return;
    }
  }
  DATA.cell_dat[shift_pos.Y][shift_pos.X].value=DATA.MEM_VAL;
  DATA.cell_dat[shift_pos.Y][shift_pos.X].type=DATA.MEM_TYPE;

  while(cell_store.type!=FREE){
    shift_pos= shift_coord(shift_pos,dir);
    shift_pos=loop_coord(shift_pos);
    if(coord_equals(shift_pos,DATA.ID_POS)){
      break;
    }
    int value_temp=cell_store.value;
    celltype type_temp=cell_store.type;
    cell_store.value=DATA.cell_dat[shift_pos.Y][shift_pos.X].value;
    cell_store.type=DATA.cell_dat[shift_pos.Y][shift_pos.X].type;
    if((type_temp==NUM)&&(DATA.cell_dat[shift_pos.Y][shift_pos.X].type==NUM)){
      //printf("\nBOTH NUMERICAL\n");
      if(value_temp<DATA.cell_dat[shift_pos.Y][shift_pos.X].value){
        DATA.cell_dat[shift_pos.Y][shift_pos.X].value=DATA.cell_dat[shift_pos.Y][shift_pos.X].value-value_temp;
        DATA.cell_dat[shift_pos.Y][shift_pos.X].type=NUM;
        break;
      }
    }
    DATA.cell_dat[shift_pos.Y][shift_pos.X].value=value_temp;
    DATA.cell_dat[shift_pos.Y][shift_pos.X].type=type_temp;
  }


}

bool interpret(){
  int id_value= DATA.cell_dat[DATA.ID_POS.Y][DATA.ID_POS.X].value;
  celltype id_type= DATA.cell_dat[DATA.ID_POS.Y][DATA.ID_POS.X].type;
  if(id_type==NUM){
    if(DATA.STORING){
      DATA.MEM_VAL=id_value;
      DATA.MEM_TYPE=NUM;
      DATA.STORING=false;
    }
  }
 char prc;
 char pst[10];
 char user[10];
  if(id_type==CHAR){
    if(DATA.STORING){
      DATA.MEM_VAL=id_value;
      DATA.MEM_TYPE=CHAR;
      DATA.STORING=false;
    }else{
      switch(id_value){
        case '>':
          DATA.dir=RIGHT;
          break;
        case 'v':
          DATA.dir=DOWN;
          break;
        case '<':
          DATA.dir=LEFT;
          break;
        case '^':
          DATA.dir=UP;
          break;
        case 'r':
          popStack(RIGHT);
          break;
        case 'd':
          popStack(DOWN);
          break;
        case 'l':
          popStack(LEFT);
          break;
        case 'u':
          popStack(UP);
          break;
        case 'R':
          pushStack(RIGHT);
          break;
        case 'D':
          pushStack(DOWN);
          break;
        case 'L':
          pushStack(LEFT);
          break;
        case 'U':
          pushStack(UP);
          break;
        case 'S':
          DATA.STORING=true;
          break;
        case 'K':
          fgets(user,2,stdin);
          DATA.MEM_VAL=user[0] ;
          break;
        case 'p':
          prc=DATA.MEM_VAL;
          strncat(DATA.output_store,&prc,1);
          break;
        case 'P':
          // strncpy(pst, "hello", 10);
          sprintf(pst,"%d",DATA.MEM_VAL);
          strcat(DATA.output_store,pst);
          break;
        case 'Q':
          return true;
          break;
      }
    }
  }
  DATA.ID_POS= shift_coord(DATA.ID_POS,DATA.dir);
  //LOOPING AROUND IN X AN Y
  DATA.ID_POS=loop_coord(DATA.ID_POS);
  return false;
}
int main(){
    char filename[20]= "tests.sk";
    if(ask_for_filename){
      printf("Please Input filename \n");
      scanf("%s", filename);
      char *ext;
      ext = strrchr(filename, '.');
      if (!ext) {
      /* no extension */
          printf("not a file\n");
          return 0;
      } else {
        if( strcmp(ext+1,"sk")!=0){
          printf("extension is %s, not sk\n", ext + 1 );
          return 0;
        }
      }
    }
    printf("loading %s \n",filename);


    FILE *file_ptr = fopen(filename, "r"); // read only
    if (file_ptr == NULL){
      printf("could not load to file (no access/dne) \n");
      return 0;
    }
    char c;
    DATA.line_count=0;
    DATA.num_columns=0;
    DATA.ID_POS.X=0;
    DATA.ID_POS.Y=0;
    DATA.MEM_VAL=0;
    DATA.MEM_TYPE=NUM;
    DATA.dir=RIGHT;


    int line_width=0;
    // DATA.cell_txt[100][1000];
    // DATA.cell_dat.value[100][1000];
    // DATA.cell_dat.type[100][1000];
    while(1) {
      c = fgetc(file_ptr);
      strncat(DATA.cell_txt[DATA.line_count],&c,1);
      int this_value= c;
      celltype this_type=CHAR;
      if(c==' '|| c=='\n'){
        this_type=FREE;
      }
      switch(c){
        case '0':
          this_type=NUM;
          this_value=0;
          break;
        case '1':
          this_type=NUM;
          this_value=1;
          break;
        case '2':
          this_type=NUM;
          this_value=2;
          break;
        case '3':
          this_type=NUM;
          this_value=3;
          break;
        case '4':
          this_type=NUM;
          this_value=4;
          break;
        case '5':
          this_type=NUM;
          this_value=5;
          break;
        case '6':
          this_type=NUM;
          this_value=6;
          break;
        case '7':
          this_type=NUM;
          this_value=7;
          break;
        case '8':
          this_type=NUM;
          this_value=8;
          break;
        case '9':
          this_type=NUM;
          this_value=9;
          break;
        case 'X':
          this_type=NUM;
          this_value=10;
          break;
        case 'C':
          this_type=NUM;
          this_value=100;
          break;
        case 'M':
          this_type=NUM;
          this_value=1000;
          break;
      }
      DATA.cell_dat[DATA.line_count][line_width].type=this_type;
      DATA.cell_dat[DATA.line_count][line_width].value=this_value;

      line_width++;
      if(c == '\n'){
        DATA.line_count++;
        if(line_width>DATA.num_columns){
          DATA.num_columns=line_width;
        }
        line_width=0;
      }
      if( feof(file_ptr) ) {
        break ;
      }
     }

    printf("line_count: %d , num_columns: %d\n Your code:\n",DATA.line_count,DATA.num_columns);
    for(int i=0; i<DATA.line_count;i++){
      printf("%s", DATA.cell_txt[i]);
    }
    printf("\nCellValues: \n");
    render();
    printf("\nCelltypes: \n");
    for(int i=0;i<DATA.line_count;i++){
      for(int j=0; j<DATA.num_columns;j++){
        if(DATA.cell_dat[i][j].type==CHAR){printf("C");}
        if(DATA.cell_dat[i][j].type==NUM){printf("N");}
        if(DATA.cell_dat[i][j].type==FREE){printf("F");}
      }
      printf("\n");
    }
    printf("\n Running Code...\n");
    bool running= true;
    while(running){
      printf("Memory Value:");
      if(DATA.MEM_TYPE==CHAR){
        printf(" CHAR %c  ",DATA.MEM_VAL);
      }else if(DATA.MEM_TYPE==NUM){
        printf(" NUM %d   ",DATA.MEM_VAL);
      }else{
        printf(" FREE %d   ",DATA.MEM_VAL);
      }
      switch(DATA.dir){
        case RIGHT:
          printf("Direction:>  ");
          break;
        case DOWN:
          printf("Direction:v  ");
          break;
        case LEFT:
          printf("Direction:<  ");
          break;
        case UP:
          printf("Direction:^  ");
          break;
      }
      printf("ID_POS.X: %d ID_POS.Y: %d \n",DATA.ID_POS.X,DATA.ID_POS.Y);
      bool halt = interpret();
      if(halt){
        running=false;
      }
      render();
      // for(int i=0;i<DATA.line_count;i++){
      //   for(int j=0; j<DATA.num_columns;j++){
      //     if(DATA.cell_dat[i][j].type==CHAR){printf("C");}
      //     if(DATA.cell_dat[i][j].type==NUM){printf("N");}
      //     if(DATA.cell_dat[i][j].type==FREE){printf("F");}
      //   }
      //   printf("\n");
      // }
      printf("Output: %s",DATA.output_store);
      printf("\n");
      usleep(1000000*0.005 );
    }
    printf("Program Halted\n STACKOMP interpreter made by MARTIN ASDF\n\n");
    return 0;
}
