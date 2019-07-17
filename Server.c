#include <stdio.h>//------------------------------------------------------------Basic I/O
#include <GL/gl.h>//------------------------------------------------------------OPENGL libraries
#include <GL/glu.h>//-----------------------------------------------------------OPENGL utility libraries
#include <GL/glut.h>//----------------------------------------------------------OPENGL toolkit libraries
#include <stdlib.h>//-----------------------------------------------------------Standard Variable Types
#include <sys/types.h>//--------------------------------------------------------System call data types
#include <sys/socket.h>//-------------------------------------------------------Socket definations
#include <string.h>//-----------------------------------------------------------String manipulation
#include <netinet/in.h>//-------------------------------------------------------Domain address Structures
#include <arpa/inet.h>//--------------------------------------------------------Inernet Operations
#include <unistd.h>//-----------------------------------------------------------Socket Read & write
#include <pthread.h>//----------------------------------------------------------Multi-threading library
#include <math.h>//-------------------------------------------------------------Maths operations

#define PORT 4444//-------------------------------------------------------------PORT number
#define BUF_SIZE 2000//---------------------------------------------------------Buffer Size
#define CLADDR_LEN 100//--------------------------------------------------------Ip length

struct sockaddr_in addr, cl_addr;
int sockfd, len, ret, newsockfd;
char buffer[BUF_SIZE];
pid_t childpid;
char clientAddr[CLADDR_LEN];
pthread_t rThread;
int lineon;

char *rectext;

int arr[1000][3], counter=0;
int front=0, end=0, cfront=0, cend=0;

char carr[100][100];

float red=0.0, green=0.0, blue=0.0;
int newsockfd;

int initsize;
char delim[]="+";
char delim2[]=".";


int linePointCount=0, circlePointCount=0, rectPointCount=0;
int rLinex, rLiney, rCirclex, rCircley, rectx, recty;

int px, py, prx, pry, x, y;

int H, W;

int brushSize=5;
int recbrushSize=5;
char keystroke, input[1024]="", temp[1024]="";
int X_coord=0, line_on=0;
char ctool[100]="IDLE";

int textx, texty;

int r=0,g=0,b=0;

unsigned char color[3];

int ml_x = 0, ml_y = 0, mc_x=0, mc_y=0;

float m;

int lpoint_no = 0, cpoint_no=0;
int X1, Y1, X2, Y2, c;
int radius;

//------------------------------------------------------------------------------Structure to Store Mouse Coordinates
struct Mouse {
	int x;
	int y;
	int lmb;
	int mmb;
	int rmb;
	int xpress;
	int ypress;
};
typedef struct Mouse Mouse;

int tool=0;

Mouse TheMouse = {0,0,0,0,0};

typedef void (*ButtonCallback)();//---------------------------------------------Button Callback Function declaration

//------------------------------------------------------------------------------Structure For Tool Buttons
struct Button {
	int   x;
	int   y;
	int   w;
	int   h;
	int state;
	int highlighted;
	char* label;
	char* discription;
	ButtonCallback callbackFunction;
};
typedef struct Button Button;

void sendMessage(int, int, int);

void clrscr(){//----------------------------------------------------------------Clear screen callback function
	X1=0;
	Y1=0;
	px=0;
	py=0;
	prx=0;
	pry=0;
	lpoint_no=-1;
	cpoint_no=-1;
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
}

void freedraw(){//--------------------------------------------------------------Pencil callback function
	if (tool!=1){
		tool=1;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "PENCIL");
	}
}

void text(){//------------------------------------------------------------------text callback function
	if (tool!=2){
		tool=2;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "TEXT");
	}
}

void line(){//------------------------------------------------------------------Straight line callback function
	if (tool!=3){
		tool=3;
		cpoint_no=0;
		X1=0;
		Y1=0;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "LINE");
	}
}

void erase(){//-----------------------------------------------------------------Eraser callback function
	if (tool!=4){
		tool=4;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "ERASER");
	}
}

void drawcir(){//---------------------------------------------------------------Circle callback function
	if (tool!=5){
		tool=5;
		lpoint_no=0;
		X1=0;
		Y1=0;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "CIRCLE");
	}
}

void drawrect(){//--------------------------------------------------------------Rectangle callback function
	if (tool!=6){
		tool=6;
		X1=0;
		Y1=0;
		memset(ctool, 0, sizeof(ctool));
		strcpy(ctool, "RECTANGLE");
	}
}

void incpoint(){//--------------------------------------------------------------Increase point size callback function
	if(brushSize<15)
		brushSize++;
}

void decpoint(){//--------------------------------------------------------------Decrease point size callback function
	if(brushSize>1)
		brushSize--;
}

//------------------------------------------------------------------------------Button variables
Button MyButton0 = {10, 40, 110, 25,0,0,"CLEAR", "Click to Clear the whole screen", clrscr};
Button MyButton1 = {10, 70, 50,25, 0,0, "PENCIL", "Free draw using Drag", freedraw};
Button MyButton2 = {10, 100, 50,25, 0,0, "TEXT", "Print text using keyboard", text};
Button MyButton3 = {70, 100, 50,25, 0,0, "LINE", "", line};
Button MyButton4 = {70, 70, 50,25, 0,0, "ERASER", "", erase};
Button MyButton5 = {10, 130, 50,25, 0,0, "CIRCLE", "", drawcir};
Button MyButton6 = {70, 130, 50,25, 0,0, "RECT", "", drawrect};
Button MyButton7 = {10, 465, 50,25, 0,0, "+", "", incpoint};
Button MyButton8 = {70, 465, 50,25, 0,0, "-", "", decpoint};

//------------------------------------------------------------------------------Function to render text
void show(int x, int y, char *string, void *font){
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
  glFlush();
}

//------------------------------------------------------------------------------Function to render single pixel
void draw_pixel(int x, int y){
	glBegin(GL_POINTS);
	glColor3ub(r,g,b);
	glVertex2i(x, y);
	//glColor3f(1.0,1.0,1.0);
	glEnd();
}

//------------------------------------------------------------------------------Function to send update
void sendMessage(int t, int x, int y){
	char xcoord[50], ycoord[50], tl[50];
	sprintf(xcoord, "%d", x);
	sprintf(ycoord, "%d", y);
	sprintf(tl, "%d", t);
	strcat(xcoord,"+");
	strcat(xcoord, ycoord);
	strcat(xcoord,"+");
	strcat(xcoord, tl);
	printf("SEND: %s\n", xcoord);
  sendto(newsockfd, xcoord, strlen(xcoord),0, (struct sockaddr *) &cl_addr, len);
}

//------------------------------------------------------------------------------Function to draw Simple Line
void drawLine(int x, int y, int r1, int g1, int b1, int axis){
	glBegin(GL_POINTS);
  glColor3f(r1, g1, b1);
  glVertex2f(x, y);
	if(axis==1){
		for(int i=0;i<100;i++){
			glVertex2f(x+i,y);
		}
	}else if(axis==2){
		for(int i=0;i<100;i++){
			glVertex2f(x,y+i);
		}
	}else if(axis==3){
		for(int i=0;i<100;i++){
			glVertex2f(x-i,y);
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);
  glEnd();
}

//------------------------------------------------------------------------------Function to draw circle using mid
void circle(int ax, int bx,int ay, int by){																		//point algorithm
	int cx = (ax+bx)/2;
	int cy = (ay+by)/2;
	int dx = bx-cx;
	int dy = by-cy;
	int g = dx*dx + dy*dy;
	radius = sqrt(g);
	int x = radius, y = 0;
	float p = 1-radius;
	while(x>y){
		y++;
		if(p<=0){
			p+=2*y+1;
		}else{
			x--;
			p+=2*y+1-2*x;
		}
		if(x<y){
			break;
		}

		if(x+cx>130)
			draw_pixel(x+cx,(y+cy));
		if(-x+cx>130)
			draw_pixel(-x+cx,(y+cy));
		if(x+cx>130)
			draw_pixel(x+cx,(-y+cy));
		if(-x+cx>130)
			draw_pixel(-x+cx,(-y+cy));
		if(y+cx>130)
			draw_pixel(y+cx,(x+cy));
		if(-y+cx>130)
			draw_pixel(-y+cx,(x+cy));
		if(y+cx>130)
			draw_pixel(y+cx,(-x+cy));
		if(-y+cx>130)
			draw_pixel(-y+cx,(-x+cy));
	}
}

//------------------------------------------------------------------------------Function to draw line using
void draw_line(int x1, int x2, int y1, int y2){																//bresenham's algorithm
	int dx, dy, i, e;
	int incx, incy, inc1, inc2;
	int x,y;

	dx = x2-x1;
	dy = y2-y1;
	if(dx!=0&&dy!=0){
		if (dx < 0)
			dx = -dx;
		if (dy < 0)
			dy = -dy;
		incx = 1;
		if (x2 < x1)
			incx = -1;
		incy = 1;
		if (y2 < y1)
			incy = -1;
		x = x1;
		y = y1;

		if (dx > dy) {
  		draw_pixel(x, y);
    	e = 2 * dy-dx;
    	inc1 = 2*(dy-dx);
    	inc2 = 2*dy;
    	for (i=0; i<dx; i++) {
      	if (e >= 0) {
					y += incy;
        	e += inc1;
      	}else
      		e += inc2;
      	x += incx;
      	draw_pixel(x, y);
    	}
		}else {
  		draw_pixel(x, y);
    	e = 2*dx-dy;
  		inc1 = 2*(dx-dy);
  		inc2 = 2*dx;
    	for (i=0; i<dy; i++) {
    		if (e >= 0) {
    			x += incx;
       		e += inc1;
    		}else
    			e += inc2;
  			y += incy;
    		draw_pixel(x, y);
			}
		}
	}else if(dx==0){				//For slope = infinite
		m=1.0;
		int pdy;
		if (dy<0){
			pdy=-dy;
			for(int i=0;i<=pdy;i++)
				draw_pixel(x1,y1-i);
		}else{
			pdy=dy;
			for(int i=0;i<=pdy;i++)
				draw_pixel(x1,y1+i);
		}
	}else if(dy==0){				//For slope = 0
		int pdx;
		if (dx<0){
			pdx=-dx;
			for(int i=0;i<=pdx;i++)
				draw_pixel(x1-i,y1);
		}else{
			pdx=dx;
			for(int i=0;i<=pdx;i++)
				draw_pixel(x1+i,y1);
		}
	}
}

//------------------------------------------------------------------------------Function to draw Rectangle
void draw_rect(int x1, int x2, int y1, int y2, int check) {
	glColor3ub(r,g,b);
	if(check==0)
		glLineWidth(brushSize);
	else
		glLineWidth(recbrushSize);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
	glFlush();
}

//------------------------------------------------------------------------------Function to Check whether
int ButtonClickTest(Button* b,int x,int y) {																	//user clicked a button or not
	if(b) {
		if( x > b->x && x < b->x+b->w && y > b->y && y < b->y+b->h ) {
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------Function to call callback function
void ButtonRelease(Button *b,int x,int y){																		//of button
	if(b) {
		if( ButtonClickTest(b,TheMouse.xpress,TheMouse.ypress)
		&& ButtonClickTest(b,x,y)){
			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}
		b->state = 0;
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------------------Function to change colour of button
void ButtonPress(Button *b,int x,int y){																			//on press to make it look like actual
	if(b){																																			//button
		if(ButtonClickTest(b,x,y)){
			b->state = 1;
      if (b==&MyButton0)
				sendMessage(0, x, y);
      if (b==&MyButton7)
				sendMessage(6, brushSize, y);
      if (b==&MyButton8)
				sendMessage(7, brushSize, y);
		}
	}
}

//------------------------------------------------------------------------------Function to change color of button
void ButtonPassive(Button *b,int x,int y){																		//on hover
	if(b){
		if( ButtonClickTest(b,x,y) ){
			if( b->highlighted == 0 ) {
				b->highlighted = 1;
				glutSwapBuffers();
				glutPostRedisplay();
			}
		}else
			if( b->highlighted == 1 )
			{
				b->highlighted = 0;
				glutSwapBuffers();
				glutPostRedisplay();
			}
	}
}

//------------------------------------------------------------------------------Function to render toolkit buttons
void ButtonDraw(Button *b){
	int fontx;
	int fonty;
	if(b){
		if (b->highlighted)
			glColor3f(0.921f,0.458f,0.188f);
		else
			glColor3f(0.341f,0.341f,0.31f);

		glBegin(GL_QUADS);
		glVertex2i( b->x     , b->y      );
		glVertex2i( b->x     , b->y+b->h );
		glVertex2i( b->x+b->w, b->y+b->h );
		glVertex2i( b->x+b->w, b->y      );
		glEnd();
		glLineWidth(3);

		if (b->state)
			glColor3f(0.3f,0.3f,0.3f);
		else
			glColor3f(0.5f,0.5f,0.5f);

		glBegin(GL_LINE_STRIP);
		glVertex2i( b->x+b->w, b->y      );
		glVertex2i( b->x     , b->y      );
		glVertex2i( b->x     , b->y+b->h );
		glEnd();

		if (b->state)
			glColor3f(0.5f,0.5f,0.5f);
		else
			glColor3f(0.3f,0.3f,0.3f);

		glBegin(GL_LINE_STRIP);
		glVertex2i( b->x     , b->y+b->h );
		glVertex2i( b->x+b->w, b->y+b->h );
		glVertex2i( b->x+b->w, b->y      );
		glEnd();

		glLineWidth(1);
		fontx = b->x + (b->w-glutBitmapLength(GLUT_BITMAP_HELVETICA_10,b->label))/2;
		fonty = b->y + (b->h+10)/2;

		if (b->state) {
			fontx+=2;
			fonty+=2;
		}
		if(b->highlighted){
			glColor3f(0,0,0);
			show(fontx,fonty, b->label, GLUT_BITMAP_HELVETICA_10);
			glColor3f(0,0,0);
			char disc[100];
			sprintf(disc, "DISCRIPTION: %s", b->discription);
			show(140, H-6, disc, GLUT_BITMAP_HELVETICA_10);
			fontx--;
			fonty--;
		}
		glColor3f(1,1,1);
		show(fontx,fonty, b->label, GLUT_BITMAP_HELVETICA_10);
	}
}

//------------------------------------------------------------------------------Function to render color spectrum
void colorPicker(void){
	glBegin(GL_POLYGON);
	glColor3f(0,0,0);
	glVertex2f(65,H-40);
	glColor3f(1,0,0);
	glVertex2f(120,H-70);
	glColor3f(1,1,0);
	glVertex2f(120,H-130);
	glColor3f(0,1,0);
	glVertex2f(65,H-160);
	glColor3f(0,1,1);
	glVertex2f(10,H-130);
	glColor3f(0,0,1);
	glVertex2f(10,H-70);
	glEnd();
	glFlush();
}

//------------------------------------------------------------------------------Function to render whole toolkit
void Draw2D(){
	glColor3f(1,1,1);
	char menu[100] = "MENU";
	show(40, 25, menu, GLUT_BITMAP_HELVETICA_18);
	ButtonDraw(&MyButton0);
	ButtonDraw(&MyButton1);
	ButtonDraw(&MyButton2);
	ButtonDraw(&MyButton3);
	ButtonDraw(&MyButton4);
	ButtonDraw(&MyButton5);
	ButtonDraw(&MyButton6);
	glColor3f(1,1,1);
	char psize[100] = "POINT SIZE";
	show(10, 450, psize, GLUT_BITMAP_HELVETICA_18);
	ButtonDraw(&MyButton7);
	ButtonDraw(&MyButton8);
	glColor3f(1,1,1);
	char cpick1[100] = "COLOR", cpick2[100]="SPECTRUM";
	show(32, H-170, cpick1, GLUT_BITMAP_HELVETICA_18);
	show(15, H-20, cpick2, GLUT_BITMAP_HELVETICA_18);
	colorPicker();
	glColor3f(0,0,0);
	char dtool[100];
	sprintf(dtool, "TOOL: %s", ctool);
	show(W-350, H-6, dtool, GLUT_BITMAP_HELVETICA_10);
	char dpsize[100];
	sprintf(dpsize, "POINT SIZE: %d", brushSize);
	show(W-250, H-6, dpsize, GLUT_BITMAP_HELVETICA_10);
	char dcolor[100];
	sprintf(dcolor, "COLOR-> R:%d G:%d B:%d", r,g,b);
	show(W-150, H-6, dcolor, GLUT_BITMAP_HELVETICA_10);
}

//------------------------------------------------------------------------------To render Welcome screen
void welcome(){
	char welcstr1[100]="WELCOME TO REAL-TIME";
	char welcstr2[100]="COLLABORATIVE PAINT";
	char welcstr3[100]="LOADING...3";

	glColor3f(1,1,1);

	show((W/2)-150,(H/2)-20,welcstr1, GLUT_BITMAP_TIMES_ROMAN_24);
	show((W/2)-145,(H/2)+5,welcstr2, GLUT_BITMAP_TIMES_ROMAN_24);
	show(W-150,H-20,welcstr3, GLUT_BITMAP_HELVETICA_18);
	glClearColor(1,0,0,1);
	int i=0;
	while(i<=W){
		for (int z=5;z<=H;z+=5){
			drawLine(i,z,1,0,0,1);
		}
		i+=5;
		glFlush();
	}
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glColor3f(0,0,0);
	char welcstr4[100]="LOADING...2";
	show((W/2)-150,(H/2)-20,welcstr1, GLUT_BITMAP_TIMES_ROMAN_24);
	show((W/2)-145,(H/2)+5,welcstr2, GLUT_BITMAP_TIMES_ROMAN_24);
	show(W-150,H-20,welcstr4, GLUT_BITMAP_HELVETICA_18);
	glClearColor(1,1,0,1);
	i=0;
	while(i<=H){
		for (int z=5;z<=W;z+=5){
			drawLine(z,i,0,1,0,2);
		}
		i+=5;
		glFlush();
	}
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glColor3f(0,0,0);
	char welcstr5[100]="LOADING...1";
	show((W/2)-150,(H/2)-20,welcstr1, GLUT_BITMAP_TIMES_ROMAN_24);
	show((W/2)-145,(H/2)+5,welcstr2, GLUT_BITMAP_TIMES_ROMAN_24);
	show(W-150,H-20,welcstr5, GLUT_BITMAP_HELVETICA_18);
	glClearColor(1,1,1,1);

	i=W;
	while(i>=0){
		for (int z=5;z<=H;z+=5){
			drawLine(i,z,0,0,1,3);
		}
		i-=5;
		glFlush();
	}
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

//------------------------------------------------------------------------------To render toolkit section
void drawSection(){
	glBegin(GL_QUADS);
	glColor3f(0.341f,0.341f,0.31f);
	glVertex2i( 0, 0);
	glVertex2i( 130, 0);
	glColor3f(0.203f,0.207f,0.187f);
	glVertex2i( 130, H);
	glVertex2i( 0, H);
	glEnd();
	glLineWidth(3);
	glColor3f(0.2f,0.2f,0.2f);
	glBegin(GL_LINES);
	glVertex2i( 130, 0);
	glVertex2i( 130, H);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.8f,0.8f,0.8f);
	glVertex2i( 131.5, H);
	glVertex2i( 131.5, H-20);
	glColor3f(0.7f,0.7f,0.7f);
	glVertex2i( W, H-20);
	glVertex2i( W, H);
	glEnd();
	glutSwapBuffers();
	glFlush();
}

//------------------------------------------------------------------------------Display Callback function
void myDisplay(void){
	drawSection();
	Draw2D();
	glutSwapBuffers();
}

//------------------------------------------------------------------------------Reshape Callback function
void reshape(int w, int h){																										//Called each time window resizes
	H=h; W=w;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	welcome();
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

//------------------------------------------------------------------------------Function to render recieved data
void setcolour(int mouseX, int mouseY, int lineon){														//from queue
	if(lineon==1){
		glColor3f(red,green,blue);
		GLint x= mouseX;
		GLint y= mouseY;
		glRecti(x,y,x+brushSize,y+brushSize);
		prx=x;
		pry=y;
		//glutSwapBuffers();
		glFlush();
		printf("check\n");
	}else if(lineon==2){
		glLineWidth(recbrushSize);
		glBegin(GL_LINES);
		glVertex2f(prx, pry);
		glVertex2f(mouseX,mouseY);
		glEnd();
		prx=mouseX;
		pry=mouseY;
		glFlush();
	}else if(lineon==3){
		circlePointCount=0;
		rectPointCount=0;
		if(linePointCount==0){
			rLinex=mouseX;
			rLiney=mouseY;
			linePointCount=1;
		}else{
			glPointSize(recbrushSize);
			draw_line(rLinex, mouseX, rLiney, mouseY);
			linePointCount=0;
		}
	}else if(lineon==4){
		linePointCount=0;
		rectPointCount=0;
		if(circlePointCount==0){
			rCirclex=mouseX;
			rCircley=mouseY;
			circlePointCount=1;
		}else{
			glPointSize(recbrushSize);
			circle(rCirclex, mouseX, rCircley, mouseY);
			circlePointCount=0;
		}
	}
	else if(lineon==5){
		linePointCount=0;
		circlePointCount=0;
		if(rectPointCount==0){
			rectx=mouseX;
			recty=mouseY;
			rectPointCount=1;
		}else{
			glPointSize(recbrushSize);
			draw_rect(rectx, mouseX, recty, mouseY, 1);
			rectPointCount=0;
		}
	}else if(lineon==0){
		clrscr();
  }else if(lineon==6 || lineon==7){
  		recbrushSize=mouseX;
  }else if(lineon==8){
    glColor3f(1.0,1.0,1.0);
    glRecti(x-10,y-10,x+10,y+10);
    glutSwapBuffers();
    glFlush();
  }
}

//------------------------------------------------------------------------------thread function to recieve data
void * receiveMessage(void * socket) {
	int sockfd, ret;
 	char buffer[BUF_SIZE];
 	sockfd = (long) socket;
 	memset(buffer, 0, BUF_SIZE);
 	for (;;) {
		ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
  	if (ret < 0) {
   		printf("Error receiving data!\n");
  	}else {
			char *ptr = strtok(buffer, delim);
    	x=atoi(ptr);
    	ptr=strtok(NULL,delim);
    	y=atoi(ptr);
    	ptr = strtok(NULL, delim);
    	lineon=atoi(ptr);
			if(lineon==9){
		  	ptr = strtok(NULL, delim);
      	r=atoi(ptr);
		  	ptr = strtok(NULL, delim);
      	g=atoi(ptr);
		  	ptr = strtok(NULL, delim);
		  	b=atoi(ptr);
		  	printf("COLOR: %d:%d:%d\n",r,g,b );
	  	}else if(lineon==10){
      	char *rectext;
      	ptr = strtok(NULL, delim);
      	rectext=ptr;
      	strcpy(carr[cfront],rectext);
      	printf("RECIEVED:");
      	printf("x:%d\ny:%d\n", x,y);
      	arr[front][0]=x;																											//update queue with recived values
      	arr[front][1]=y;
      	arr[front][2]=lineon;
      	cfront++;
      	front++;
      	memset(buffer, 0, sizeof(buffer));
    	}else{
    		printf("RECIEVED:");
    		printf("x:%d\ny:%d\n", x,y);
    		arr[front][0]=x;
    		arr[front][1]=y;
    		arr[front][2]=lineon;
    		front++;
    		memset(buffer, 0, sizeof(buffer));
  		}
		}
	}
}

//------------------------------------------------------------------------------Idle callback function
void render(){																																//extract data from queue
  if(end<front){																															//and render it
    if (arr[end][2]==10){
      if (cend<cfront){
        show(arr[end][0],arr[end][1], carr[cend], GLUT_BITMAP_TIMES_ROMAN_24);
        cend++;
        end++;
      }else if(cend==cfront){
        cend=0;
        cfront=0;
      }
    }else{
      setcolour(arr[end][0],arr[end][1],arr[end][2]);
      end++;
    }
  }else if(end==front){
    end=0;
    front=0;
		//memset(carr, 0, sizeof(carr));
  }
}

//------------------------------------------------------------------------------Mouse Button callback function
void MouseButton(int button,int state,int x, int y){
	TheMouse.x = x;
	TheMouse.y = y;
	if (state == GLUT_DOWN) {
		if ( !(TheMouse.lmb ) ) {
			TheMouse.xpress = x;
			TheMouse.ypress = y;
		}
		if(tool==2){
			textx=x;
			texty=y;
		}
		switch(button) {
			case GLUT_LEFT_BUTTON:
				TheMouse.lmb = 1;
				ButtonPress(&MyButton0,x,y);
				ButtonPress(&MyButton1,x,y);
				ButtonPress(&MyButton2,x,y);
				ButtonPress(&MyButton3,x,y);
				ButtonPress(&MyButton4,x,y);
				ButtonPress(&MyButton5,x,y);
				ButtonPress(&MyButton6,x,y);
				ButtonPress(&MyButton7,x,y);
				ButtonPress(&MyButton8,x,y);
				break;
			case GLUT_MIDDLE_BUTTON:
				TheMouse.mmb = 1;
				break;
			case GLUT_RIGHT_BUTTON:
				TheMouse.rmb = 1;
				break;
		}
		if(x > 10 && x < 120 && y > H-160 && y < H-40){
			glReadPixels(x,H-y,1,1,GL_RGB, GL_UNSIGNED_BYTE, color);
			if(color[0]!=255 && color[1]!=255 && color[2]!=255){
				r=color[0];
				g=color[1];
				b=color[2];
				/*char xcoord[50], ycoord[50];
				char rc[5], gc[5], bc[5];
				sprintf(xcoord, "%d", x);
				sprintf(ycoord, "%d", y);
				sprintf(rc, "%d", r);
				sprintf(gc, "%d", g);
				sprintf(bc, "%d", b);
				strcat(xcoord,"+");
				strcat(xcoord,ycoord);
				strcat(xcoord,"+");
				strcat(xcoord,"9");
				strcat(xcoord,"+");
				strcat(xcoord,rc);
				strcat(xcoord,"+");
				strcat(xcoord,gc);
				strcat(xcoord,"+");
				strcat(xcoord,bc);
				//printf("Send: %s\n",xcoord );
				//sendto(newsockfd , xcoord , strlen(xcoord) , 0, (struct sockaddr *) &cl_addr, len );*/
			}
		}
	}else{
		switch(button) {
			case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 0;
			ButtonRelease(&MyButton0,x,y);
			ButtonRelease(&MyButton1,x,y);
			ButtonRelease(&MyButton2,x,y);
			ButtonRelease(&MyButton3,x,y);
			ButtonRelease(&MyButton4,x,y);
			ButtonRelease(&MyButton5,x,y);
			ButtonRelease(&MyButton6,x,y);
			ButtonRelease(&MyButton7,x,y);
			ButtonRelease(&MyButton8,x,y);
			break;
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 0;
			break;
		}
	}
	if(tool==3 && TheMouse.x>130 && TheMouse.y<H-20){
		if (button==GLUT_LEFT_BUTTON){
			if (state==GLUT_DOWN){
				if(lpoint_no==-1)
					lpoint_no=0;
				ml_x = x;
				ml_y = y;
			}else if(state == GLUT_UP){
				sendMessage(3,ml_x,ml_y);
        sendMessage(3,x,y);
        glPointSize(brushSize);
        draw_line(ml_x, x, ml_y, y);
      }
		}
	}
	if(tool==1 && TheMouse.x>130 && TheMouse.y<H-20){
		if (button==GLUT_LEFT_BUTTON){
			if (state==GLUT_DOWN){
				line_on==0;
				sendMessage(1,x,y);
		    px=x;
		    py=y;
		    glColor3f(red,green,blue);
		    glRecti(x,y,x+brushSize,y+brushSize);
			}
		}
	}
	if(tool==5 && TheMouse.x>130 && TheMouse.y<H-20){
		if (button==GLUT_LEFT_BUTTON){
			if (state==GLUT_DOWN){
				if(cpoint_no==-1)
					cpoint_no=0;
       				mc_x = x;
       				mc_y = y;
			}else if (state==GLUT_UP) {
				sendMessage(4,mc_x, mc_y);
        sendMessage(4,x,y);
        glPointSize(brushSize);
        circle(mc_x, x, mc_y, y);
      }
		}
	}
	if(tool==6 && TheMouse.x>130 && TheMouse.y<H-20){
		if (button==GLUT_LEFT_BUTTON){
			if (state==GLUT_DOWN){
				ml_x = x;
				ml_y = y;
			}else if(state==GLUT_UP){
				sendMessage(5,ml_x,ml_y);
				sendMessage(5,x,y);
				glPointSize(brushSize);
				draw_rect(ml_x, x, ml_y, y, 0);
				glFlush();
			}
		}
	}
}

//------------------------------------------------------------------------------Mouse motion Callback function
void MouseMotion(int x, int y){
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;
	if (tool==1 && x >130 && y<H-20){
		glColor3ub(r,g,b);
		if (line_on==0){
    	glRecti(x,y,x+brushSize,y+brushSize);
		}else{
			sendMessage(2, x, y);
			glLineWidth(brushSize);
			glBegin(GL_LINES);
				glVertex2i(px, py);
				glVertex2i(x,y);
			glEnd();
			px=x;
	    py=y;
		}
		glutSwapBuffers();
    glFlush();
	}else if (tool==4 && TheMouse.lmb && x>140 && y<H-30){
    sendMessage(8, x, y);
		glColor3f(1.0,1.0,1.0);
		glRecti(x-10,y-10,x+10,y+10);
		glutSwapBuffers();
		glFlush();
		//glutPostRedisplay();
	}
	TheMouse.x = x;
	TheMouse.y = y;
	line_on=1;
}

//------------------------------------------------------------------------------Passive motion Callback function
void MousePassiveMotion(int x, int y){
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;
	TheMouse.x = x;
	TheMouse.y = y;
	ButtonPassive(&MyButton0,x,y);
	ButtonPassive(&MyButton1,x,y);
	ButtonPassive(&MyButton2,x,y);
	ButtonPassive(&MyButton3,x,y);
	ButtonPassive(&MyButton4,x,y);
	ButtonPassive(&MyButton5,x,y);
	ButtonPassive(&MyButton6,x,y);
	ButtonPassive(&MyButton7,x,y);
	ButtonPassive(&MyButton8,x,y);
}

//------------------------------------------------------------------------------Keyboard Callback function
void myKeboard(unsigned char theKey, int mouseX, int mouseY){
  if(tool==2 && textx>130 && texty<H-20 ){
		int i=0, j=0, len, fchar;
		if (X_coord != textx){
      memset(input, 0, sizeof(input));
  		X_coord = mouseX;
		}
		glColor3ub(r,g,b);
		glRasterPos2f(textx,texty);
		//printf("THE KEY:%c", theKey);
		if(theKey==127 || theKey==8){
		  int delwid = 0;
	    len = strlen(input);
			for (i = 0; i <len-1; i++)
			  delwid+=(int)glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, input[i]);
      fchar=(int)glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, input[len-1]);
	    glScissor(textx+delwid,700-texty-5,fchar,25);
		  glEnable(GL_SCISSOR_TEST);
  	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  glDisable(GL_SCISSOR_TEST);
	    if(strlen(input)>0){
        for(int i=0;i<strlen(input)-1;i++)
			    temp[j++]=input[i];
	       strcpy(input, temp);
  			 memset(temp, 0, sizeof(temp));
      }
    }else{
  		keystroke = (char)theKey;
  		len = strlen(input);
  		input[len]=keystroke;
		}
		len = strlen(input);
		for (i = 0; i <len; i++)
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, input[i]);
    char xcoord[50], ycoord[50];
    sprintf(xcoord, "%d", mouseX);
    sprintf(ycoord, "%d", mouseY);
    strcat(xcoord,"+");
    strcat(xcoord,ycoord);
    strcat(xcoord,"+");
    strcat(xcoord,"10");
    strcat(xcoord,"+");
    strcat(xcoord,input);
    printf("Send: %s\n",xcoord );
    sendto(newsockfd, xcoord, strlen(xcoord), 0, (struct sockaddr *) &cl_addr, len);
	  glutSwapBuffers();
		glFlush();
	}
}

void main(int argc, char **argv) {
	//----------------------------------------------------------------------------initialization of glut library
  	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  	glutInitWindowSize(1150,700);
  	glutInitWindowPosition(200,50);
  	glutCreateWindow("SERVER");//-------------------------------------------------initialize window
  	glutReshapeFunc(reshape);
  	glutDisplayFunc(myDisplay);//-------------------------------------------------Display handler
  	glutMouseFunc(MouseButton);//-------------------------------------------------Mouse Button handler
  	glutMotionFunc(MouseMotion);//------------------------------------------------Mouse Motion handler
  	glutPassiveMotionFunc(MousePassiveMotion);//----------------------------------Mouse Passive Motion handler
  	glutKeyboardFunc(myKeboard);//------------------------------------------------Keyboard handler

	//----------------------------------------------------------------------------Socket creation
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);																		//AF_INET for IPv4
 	if (sockfd < 0) {																														//SOCK_STREAM for TCP
  		printf("Error creating socket!\n");
  		exit(1);
 	}
 	printf("Socket created\n");

	//----------------------------------------------------------------------------Binding address to the Socket
	memset(&addr, 0, sizeof(addr));
 	addr.sin_family = AF_INET;
 	addr.sin_addr.s_addr = INADDR_ANY;
 	addr.sin_port = PORT;
 	ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
 	if (ret < 0) {
  		printf("Error binding!\n");
  		exit(1);
 	}
	printf("Socket binded\n");

	//----------------------------------------------------------------------------Listening for incomming request
	printf("Listening.......\n");
 	listen(sockfd, 5);

	//----------------------------------------------------------------------------Accepting requests
 	len = sizeof(cl_addr);
 	newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);
 	if (newsockfd < 0) {
  		printf("Error accepting connection!\n");
  		exit(1);
 	}
	printf("Connected\n");

 	inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
 	memset(buffer, 0, BUF_SIZE);

	//----------------------------------------------------------------------------Thread For recieving data from client
 	ret=pthread_create(&rThread,NULL,receiveMessage,(void *)(intptr_t)newsockfd);
 	if (ret) {
  		printf("ERROR: Return Code from pthread_create() is %d\n", ret);
  		exit(1);
 	}
	printf("Thread created\n");

 	glutIdleFunc(render);	//------------------------------------------------------Idle Function handler
 	glutMainLoop();	//------------------------------------------------------------GLUT event processing loop
}

