#include<allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<math.h>

#define scrx 1024
#define scry 720
#define tile_x 32
#define tile_y 32
#define mapheight 30
#define mapwidth 50
#define G 300
#define t_length 10000
#define WHITE makecol(255, 255, 255)
#define GREEN makecol(0, 255, 0)
#define RED makecol(255, 0, 0)
#define BLUE makecol(0, 0, 255)
#define YELLOW makecol(255, 255, 0)
#define LBLUE makecol(0, 255, 255)
#define ORANGE makecol(255, 128, 0)

using namespace std;

volatile long speed_counter = 0;
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter);

struct object
{
    int x;
    int y;
    int mass;
};

struct player
{
    float x;
    float y;
    float xspeed;
    float yspeed;
    int mass;
    float a;
    int ax;
    int ay;
    int x1[t_length];
    int x2[t_length];
    int xt[t_length];
    int yt[t_length];
    int y1[t_length];
    int y2[t_length];
    bool upd[t_length];
};

bool collide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

int main(int argc, char *argv[]){
    
    allegro_init();
    install_keyboard();
    install_mouse(); 
    install_timer();
    set_color_depth(desktop_color_depth());
    install_sound (MIDI_AUTODETECT, MIDI_AUTODETECT , 0); //Needed to play Music
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, scrx, scry, 0, 0);
    LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));
    COLOR_MAP global_light_table;
    srand(time(NULL));
    set_trans_blender(0, 0, 0, 0);
    
    BITMAP *buffer = create_bitmap(scrx, scry);
    
    //sprite
    BITMAP *o1 = load_bitmap("object.bmp", NULL);
    BITMAP *p1 = load_bitmap("player.bmp", NULL);
    //variable
    object o;
    player p;
    int pos_upd=0;
    int x_length;
    int y_length;
    bool show = true;
    bool sticky = false;
    p.x=scrx-550;
    p.xspeed=-75;
    p.mass=100;
    p.yspeed=-200;
    p.y=scry-400;
    o.x=300;
    o.y=300;
    o.mass=500;
    for(int i=0;i<t_length;i++)
    {
    	p.upd[i]=false;
    }
    p.xt[0]=p.x+p1->w/2;
    p.yt[0]=p.y+p1->h/2;
    int pos_incr=0;
    
    float a=0;
    float r;
    float Fg;
    
    bool initialize = true;

    while(!key[KEY_ESC])
    {
        while(speed_counter > 0)
        {
            if(key[KEY_D])
            { 
                    p.x+=10;
    		}
    		else if(key[KEY_A])
            { 
                   p.x-=10;
    		}
    		else if(key[KEY_W])
            {
                    p.y-=10;
    		} 
    		else if(key[KEY_S])
            {
                p.y+=10;
            }     
			else if(key[KEY_RIGHT])
            { 
                    p.xspeed+=7;
    		}
    		else if(key[KEY_LEFT])
            { 
                   p.xspeed-=7;
    		}
    		else if(key[KEY_UP])
            {
                    p.yspeed-=7;
    		} 
    		else if(key[KEY_DOWN])
            {
                	p.yspeed+=7;
            }                            
    		else if(key[KEY_X])
            {
                for(int i=0;i<t_length;i++)
			    	p.upd[i]=false;
            }   
			else if(key[KEY_Z])
            {
                p.xspeed=0;
                p.yspeed=0;
            }    
			else if(key[KEY_SPACE] && sticky == false)
            {
            	sticky = true;
                if(show == true)
                	show = false;
                else
                	show = true;
            }                   
            
            if(!key[KEY_SPACE] && sticky == true)
            	sticky = false;
            //gravity------------------------------------------------------------------------------------
            r=sqrt(pow(abs((p.x+p1->w/2)-(o.x+o1->w/2)),2)+pow(abs((p.y+p1->h/2)-(o.y+o1->h/2)),2))*0.18;
            Fg=(G*p.mass*o.mass)/(pow(r,2));
            p.a=Fg/p.mass;
            
            //track force----------------------------------------
            
			x_length=o.x+o1->w/2-(p.x+p1->w/2);
			y_length=o.y+o1->h/2-(p.y+p1->h/2);
			
			if(x_length>=0)
			{
				if(x_length!=0)
                	p.ax = p.a/sqrt((1+y_length*y_length/(x_length*x_length)));
                else 
					p.ax = 0;
        	}
            else
                p.ax = -p.a/sqrt((1+y_length*y_length/(x_length*x_length)));
            if(y_length>=0)
			{
				if(y_length!=0)
                	p.ay = p.a/sqrt((1+x_length*x_length/(y_length*y_length)));
                else 
					p.ay = 0;
        	}
            else
                p.ay = -p.a/sqrt((1+x_length*x_length/(y_length*y_length)));
            p.yspeed+=0.01*p.ay;
            p.xspeed+=0.01*p.ax;
            //movements-------------------------------------------
            for(int i = 0;i<abs(int(p.yspeed));i++)
		    {
                if(p.yspeed>=0)
                {
                    if(!collide(p.x,p.y+1,p1->w, p1->h,o.x,o.y,o1->w, o1->h))
                        p.y+=0.01;
                    else
                        break;
                }
                if(p.yspeed<=0)
                {
                    if(!collide(p.x,p.y-1,p1->w, p1->h,o.x,o.y,o1->w, o1->h))
                        p.y-=0.01;
                    else
                        break;
                }
            }
            
            for(int i = 0;i<abs(int(p.xspeed));i++)
		    {
                if(p.xspeed>=0)
                {
                    if(!collide(p.x+1,p.y,p1->w, p1->h,o.x,o.y,o1->w, o1->h))
                        p.x+=0.01;
                    else break;
                }
                if(p.xspeed<=0)
                {
                    if(!collide(p.x-1,p.y,p1->w, p1->h,o.x,o.y,o1->w, o1->h))
                        p.x-=0.01;
                    else break;
                }
            }
                  
            a+=0.5;
            if(pos_upd<5)
            	pos_upd++;
            else
            {
            	pos_upd=0;
        		p.upd[pos_incr%t_length]=true;
        		p.x1[pos_incr%t_length]=p.xt[pos_incr%t_length];
        		p.y1[pos_incr%t_length]=p.yt[pos_incr%t_length];
        		p.x2[pos_incr%t_length]=p.x+p1->w/2;
        		p.y2[pos_incr%t_length]=p.y+p1->h/2;
        		p.xt[(pos_incr+1)%t_length]=p.x2[pos_incr%t_length];
        		p.yt[(pos_incr+1)%t_length]=p.y2[pos_incr%t_length];
        		pos_incr++;
        	}
        	cout << a << endl;
            speed_counter--;
        }
        rotate_sprite(buffer,p1,p.x,p.y,ftofix(a));
        rotate_sprite(buffer,o1,o.x,o.y,ftofix(a));
        
        if(show == true)
        {
	        for(int i=0;i<t_length;i++)
	        	if(p.upd[i] == true)
	        		line(buffer, p.x1[i], p.y1[i], p.x2[i], p.y2[i], GREEN);
	    	line(buffer, p.x+p1->w/2, p.y+p1->h/2, o.x+o1->w/2, o.y+o1->h/2,RED);
	    	line(buffer, p.x+p1->w/2, p.y+p1->h/2, p.x+p1->w/2+p.xspeed/2, p.y+p1->h/2+p.yspeed/2,LBLUE);
	    	line(buffer, p.x+p1->w/2, p.y+p1->h/2, p.x+p1->w/2+p.ax/2, p.y+p1->h/2+p.ay/2,YELLOW);
	        
	        if(p.x+p1->w>scrx || p.x<0 || p.y>scry ||p.y<0)
			{
		        textprintf_ex(buffer, font, scrx-200,20, WHITE, -1, "Fg: %f", Fg);
		        textprintf_ex(buffer, font, scrx-200,30, WHITE, -1, " r: %f", r);
		        textprintf_ex(buffer, font, scrx-200,40, WHITE, -1, " a: %f", p.a);
		        textprintf_ex(buffer, font, scrx-200,50, WHITE, -1, "vx: %f", p.xspeed);
		        textprintf_ex(buffer, font, scrx-200,60, WHITE, -1, "vy: %f", p.yspeed);
		        textprintf_ex(buffer, font, scrx-200,10, WHITE, -1, "m1: %d", p.mass);
	    	}
	        else
	        {
		        textprintf_ex(buffer, font, p.x,p.y-50, WHITE, -1, "Fg: %f", Fg);
		        textprintf_ex(buffer, font, p.x,p.y-40, WHITE, -1, " r: %f", r);
		        textprintf_ex(buffer, font, p.x,p.y-30, WHITE, -1, " a: %f", p.a);
		        textprintf_ex(buffer, font, p.x,p.y-20, WHITE, -1, "vx: %f", p.xspeed);
		        textprintf_ex(buffer, font, p.x,p.y-10, WHITE, -1, "vy: %f", p.yspeed);
		        textprintf_ex(buffer, font, p.x,p.y-60, WHITE, -1, "m1: %d", p.mass);
	    	}
	        textprintf_ex(buffer, font, o.x,o.y-10, WHITE, -1, "m2: %d", o.mass);
	        //cout << p.a << " " << r << " " << Fg/p.mass << endl;
    	}
        blit(buffer, screen, 0, 0, 0, 0, scrx, scry); // Blit the buffer
        clear(buffer); // Clear the buffer

    }
   	destroy_bitmap(buffer); //Release the bitmap data 
    return(0);
}END_OF_MAIN();

bool collide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    //the code is taken from allegro tutorial and made into a function
    /*
    int image1_bb_left = x1;
    int image1_bb_top = y1;
    int image1_bb_right = (image1_bb_left + w1);
    int image1_bb_bottom = (image1_bb_top + h1);
    int image2_bb_left = x2;
    int image2_bb_top = y2;
    int image2_bb_right = (image2_bb_left + w2);
    int image2_bb_bottom = (image2_bb_top + h2);
    if(image1_bb_bottom < image2_bb_top)
        return false;
    else if(image1_bb_top > image2_bb_bottom)
        return false;
    else if(image1_bb_right < image2_bb_left)
        return false;
    else if(image1_bb_left > image2_bb_right)
        return false;
    return(true);
    */
    return false;
}
END_OF_FUNCTION(collide)
