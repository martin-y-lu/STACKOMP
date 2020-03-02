#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef int boolean;
#define true 1
#define false 0

typedef enum {FREE,CHAR,NUM} celltype;
typedef enum {RIGHT,DOWN,LEFT,UP} direct;

boolean ask_for_filename = false;
boolean draw_render = true;
boolean show_debug_info = true;
int micro_sec_delay = 10000;

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
  boolean STORING;
  celltype MEM_TYPE;
  direct dir;
  char output_store[20000];
} DATA;

//COORD FUNCTIONS
boolean coord_equals(struct coord A,struct coord B){
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

void render(){
  move(1,0);
  printw("  ");
  for(int j=0; j<DATA.num_columns;j++){
    move(1,3+j);
    printw("_");
  }
  // printf("\n");
  for(int i=0;i<DATA.line_count;i++){
    move(i+2,0);
    printw("x| ");
    for(int j=0; j<DATA.num_columns;j++){
      move(i+2,3+j);
      if(i==DATA.ID_POS.Y && j==DATA.ID_POS.X){
        printw("&");
      }else if(DATA.cell_dat[i][j].type==NUM){
        printw("*");
      }else if(DATA.cell_dat[i][j].value=='\n'){
        printw("|x");
      }else{
        printw("%c",DATA.cell_dat[i][j].value);
      }
    }
  }
  move(DATA.line_count+2,0);
  printw("  ");
  for(int j=0; j<DATA.num_columns;j++){
    move(DATA.line_count+2,3+j);
    printw("_");
  }
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

boolean interpret(){
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
          if((draw_render==false)&&(show_debug_info==false)){
            scanf("%s",user);
            printf("%s\n\n\n",user);
            DATA.MEM_VAL=user[0] ;
          }else{
            DATA.MEM_VAL=getch();
          }
          break;
        case 'p':
          if((draw_render==false)&&(show_debug_info==false)){
            printf("%c",DATA.MEM_VAL);
          }else{
            prc=DATA.MEM_VAL;
            strncat(DATA.output_store,&prc,1);
          }
          break;
        case 'P':
          // strncpy(pst, "hello", 10);
          if((draw_render==false)&&(show_debug_info==false)){
            printf("%d",DATA.MEM_VAL);
          }else{
            sprintf(pst,"%d",DATA.MEM_VAL);
            strcat(DATA.output_store,pst);
          }
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
int main(int argc, char** argv){
    char filename[20]= "info.sk";
    // filename=argv[0];
    if(argc>1){
      sprintf(filename,"%s",argv[1]);
    }
    if(argc>2){
      micro_sec_delay= atoi(argv[2]);
    }
    int number=0;
    if(argc>3){
      number = atoi(argv[3]);
      if(number>0){
        draw_render=true;
      }else{
        draw_render=false;
      }
    }
    if(argc>4){
      number = atoi(argv[4]);
      if(number>0){
        show_debug_info=true;
      }else{
        show_debug_info=false;
      }
    }
    // printf(filename);
    if(ask_for_filename||(argc<2)){
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

     initscr();
    printw("line_count: %d , num_columns: %d\n Your code:\n",DATA.line_count,DATA.num_columns);

    for(int i=0; i<DATA.line_count;i++){
      printw("%s", DATA.cell_txt[i]);
    }
    printw("Any key to continue");
    refresh();
        getch();

    clear();
    printw("CellValues: \n");
    render();
    printw("Any key to continue");
    refresh();
    getch();
    clear();
    printw("Celltypes: \n");
    for(int i=0;i<DATA.line_count;i++){
      for(int j=0; j<DATA.num_columns;j++){
        move(i+2,j);
        if(DATA.cell_dat[i][j].type==CHAR){printw("C");}
        if(DATA.cell_dat[i][j].type==NUM){printw("N");}
        if(DATA.cell_dat[i][j].type==FREE){printw("F");}
      }
    }
    printw("Any key to continue");
    refresh();
    getch();
    printw("\n Running Code...\n");
    refresh();
    boolean running= true;
    boolean using_curse=draw_render||show_debug_info;
    if(!using_curse){
      endwin();
      printf("Output:");
    }
    while(running){
      if(using_curse){
        erase();
      }
      if(show_debug_info){
        printw("Memory Value:");
        if(DATA.MEM_TYPE==CHAR){
          printw(" CHAR %c  ",DATA.MEM_VAL);
        }else if(DATA.MEM_TYPE==NUM){
          printw(" NUM %d   ",DATA.MEM_VAL);
        }else{
          printw(" FREE %d   ",DATA.MEM_VAL);
        }
        switch(DATA.dir){
          case RIGHT:
            printw("Direction:>  ");
            break;
          case DOWN:
            printw("Direction:v  ");
            break;
          case LEFT:
            printw("Direction:<  ");
            break;
          case UP:
            printw("Direction:^  ");
            break;
        }
        printw("ID_POS.X: %d ID_POS.Y: %d \n",DATA.ID_POS.X,DATA.ID_POS.Y);
      }
      boolean halt = interpret();
      if(halt){
        running=false;
      }
      if(draw_render){
        render();
      }
      if(using_curse){
        printw("\nOutput: %s",DATA.output_store);
        refresh();
        printw("\n");
      }
      usleep(micro_sec_delay);
    }
    // usleep(10000000);
    if(using_curse){
      endwin();
    }
    printf("Program Halted\n");
    printf("Final Output: %s\n",DATA.output_store);
    printf("STACKOMP language and interpreter made by MARTIN ASDF\n\n");
    return 0;
}
