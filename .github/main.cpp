/* Time Manager - by Abid        */
/* First version 09/04/2020      */
/* main.cpp - main function file */

#include "awindows.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace sf;

extern window *active_window;
extern win_comp *active_comp;

bool file_io_success;

bool save(char *filename, int grid[MA_WIDTH][MA_HEIGHT])
{
    FILE *fp;
    char temp_rw;

    fp = fopen(filename,"w");
    if (fp==NULL) return file_io_success=false;

    for (int j=0; j<MA_HEIGHT; j++) {
        for (int i=0; i<MA_WIDTH; i++) {
            temp_rw=(char) grid[i][j]+'0';
            fwrite(&temp_rw,sizeof temp_rw,1,fp);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return file_io_success=true;
}

bool open(char *filename, int grid[MA_WIDTH][MA_HEIGHT])
{
    FILE *fp;
    char temp_rw;

    fp = fopen(filename,"r");
    if (fp==NULL) return file_io_success=false;
    for (int j=0; j<MA_HEIGHT; j++) {
        for (int i=0; i<MA_WIDTH; i++) {
            fread(&temp_rw,sizeof temp_rw,1,fp);
            grid[i][j]=(int)(temp_rw-'0');
        }
        fscanf(fp,"\n");
    }
    fclose(fp);
    return file_io_success=true;
}

time_t datefstr(char str[11]) {
    time_t r;
    struct tm *ptm=(struct tm*)calloc(1,sizeof (struct tm));

    for (int i=0; i<=9; i++) {
        if (i==4 || i==7) {
            if (str[i]!='-') return 0;
        } else {
            if (!isdigit(str[i])) return 0;
        }
    }
    ptm->tm_mday=(str[9]-'0')+(str[8]-'0')*10;
    ptm->tm_mon=(str[6]-'0')+(str[5]-'0')*10;
    ptm->tm_year=(str[3]-'0')+(str[2]-'0')*10+(str[1]-'0')*100+(str[0]-'0')*1000-1900;

    r=mktime(ptm)/86400;
    free(ptm);
    return r;
}

void strfdate(char *ptr, time_t date)
{
    struct tm *ptm=(struct tm*)calloc(1,sizeof (struct tm));

    date*=86400;
    ptm=localtime(&date);
    strftime(ptr, 11, "%Y-%m-%d", ptm);
    free(ptm);
}

int main()
{                                                                                               /** ==== [1] PREPARATIONS ==== */
    RenderWindow app(VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT),"Time Manager");       /* ===(1.1) Set up window, textures, etc.=== */               /* ==== [1] PREPARATIONS ==== */
    active_window = NULL;
    active_comp = NULL;

    Image icon;
    icon.loadFromFile("icon.png");
    app.setIcon(icon.getSize().x,icon.getSize().y,icon.getPixelsPtr());
    app.setPosition(Vector2i(app.getPosition().x,30));


    Texture tframe;                                                                                 /* --1.1.1 Load textures-- */
    Texture t_icons;
    Texture t_bgcolor;
    Texture t_f_unfocus;
    Texture t_selection;
    Texture t_x;
    Texture t_nowmark;

    tframe.loadFromFile("images/frame.bmp");
    t_icons.loadFromFile("images/icons.png");
    t_bgcolor.loadFromFile("images/bgcolor.png");
    t_f_unfocus.loadFromFile("images/frame_unfocus.png");
    t_selection.loadFromFile("images/selection.png");
    t_x.loadFromFile("images/x.png");
    t_nowmark.loadFromFile("images/nowmark.png");


    Sprite sframe(tframe);                                                                           /* --1.1.2 Make sprites-- */
        sframe.setTextureRect(ss_hv(0,0,MARGIN_HRZT+MA_WIDTH+MID_SEPARATOR,MARGIN_VERT+MA_HEIGHT));
        sframe.setPosition(0,0);

    Sprite s(t_icons);
    Sprite s_toolbar(t_icons);
        s_toolbar.setTextureRect(ss_vert(0,NUM_ICONS_MAIN));
        s_toolbar.setPosition(0,0);
    Sprite s_icaf(t_icons); /* icon for custom activity frequency */
        s_icaf.setTextureRect(ss_vert(CA_ZERO+11,9));
        s_icaf.setPosition(0,ICON_SIZE*(NUM_ICONS_MAIN+1));

    Sprite s_bgcolor(t_bgcolor);

    Sprite s_fu(t_f_unfocus);
        s_fu.setTextureRect(IntRect(0,0,WINDOW_WIDTH,WINDOW_HEIGHT));
        s_fu.setPosition(0,0);

    Sprite s_selection(t_selection);
        s_selection.setTextureRect(ss_vert(0));
    Sprite s_tb_sel(t_selection);
        s_tb_sel.setTextureRect(ss_vert(0));

    Sprite s_passedx(t_x);
        s_passedx.setTextureRect(ss_vert(0));

    Sprite s_nowmark(t_nowmark);
        s_nowmark.setTextureRect(ss_vert(0));


    Font font;                                                                                               /* --1.1.3 Text-- */
    Font font2;
        font.loadFromFile("fonts/pointfree.ttf");
        font2.loadFromFile("fonts/cruft.ttf");

    Text txt("",font,FREQUENCY_FONT_SIZE);
        txt.setFillColor(Color::Black);
    Text fname_txt("",font2,WIN_FONT_SIZE);
        fname_txt.setFillColor(Color::Black);
    Text datetime_txt("",font2,WIN_FONT_SIZE);
        datetime_txt.setFillColor(Color::Black);
        datetime_txt.setPosition(4,WINDOW_HEIGHT-ICON_SIZE);


    window *wptr=NULL;                                                                    /* ===(1.2) Variable declarations=== */
    int at_hand=0;
    bool ranging=false;
    int xb=-1,yb=-1;
    int grid[MA_WIDTH][MA_HEIGHT]={0};
    int dgrid[MA_WIDTH][MA_HEIGHT]={0};
    int x,y;
    int tbsp=0; /* toolbar selection position */
    char temp_num_txt[4];
    int freq_main[NUM_ICONS_MAIN];
    int freq_main2[NUM_ICONS_MAIN];
    int freq_ca[10];
    char tb_text[MAX_TEXT_LENGTH+1]={0};
    int closed_window_id=0;
    bool window_just_opened=false;
    char fname[MAX_TEXT_LENGTH+2];
    bool opened_file=false;
    bool unsaved_work=false;

    time_t current_time;
    time_t today;
    struct tm *pcurrent_tm;
    char today_string[11];
    int hour,minute;
    time_t open_date=0;
    char datetime_string[20];

    strfdate(today_string, (today=(current_time=time(NULL))/86400));               /* ===(1.3) Open/make new file for today=== */
    if (open(today_string,grid)) {
        strcpy(fname,today_string);
        opened_file=true;
        unsaved_work=false;
        open_date=today;
    } else {
        save(today_string, grid);
        if (file_io_success) {
            strcpy(fname,today_string);
            opened_file=true;
            unsaved_work=false;
            open_date=today;
        }
    }

    while (app.isOpen()) {                                                                 /** ==== [2] MAIN PROGRAM LOOP ==== */
        Vector2i pos = Mouse::getPosition(app);                /* ===(2.1) Initialize variables for mouse position and time=== */
        x = pos.x/ICON_SIZE-MARGIN_LEFT;
        y = pos.y/ICON_SIZE-MARGIN_TOP;

        strfdate(today_string, (today=(current_time=time(NULL))/86400));
        pcurrent_tm=localtime(&current_time);
        hour=pcurrent_tm->tm_hour;
        minute=pcurrent_tm->tm_min;

        Event e;                                                        /* ===(2.2) Main application logic (event handling)=== */
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) {                                       /* --(2.2.1) Application window close event-- */
                app.close();
            } else if (active_comp!=NULL) {                                 /* --(2.2.2) There is an active window component-- */
                if (active_comp->comp_id==TEXT_BOX) {                                          /** -2.2.2.1 [Text box active]- */
                    int len;
                    if (e.type == Event::TextEntered) {
                        char c = (char) e.text.unicode;
                        len = strlen(active_comp->text);
                        switch (c) {
                        case '\e':
                            break;
                        case '\n':
                        case '\r':
                            strcpy(tb_text, active_comp->text);
                            closed_window_id = wptr->window_id;
                            destroy_window((window*)(active_comp->parent));
                            wptr=NULL;
                            break;
                        case '\b':
                            if (len<=0) break;
                            active_comp->text[len-1]='\0';
                            break;
                        default:
                            if (len>=MAX_TEXT_LENGTH) break;
                            active_comp->text[len]=c;
                            active_comp->text[len+1]='\0';
                            break;
                        }
                    }
                    if (window_just_opened) {
                        if ((len=strlen(active_comp->text))>0)
                        active_comp->text[strlen(active_comp->text)-1]='\0';
                        window_just_opened=false;
                    }
                }
            } else if (closed_window_id) {                                                  /* --(2.2.3) A window was closed-- */
                switch (closed_window_id) {
                case WIN_SAVE_BOX:                                                             /** -2.2.3.1 [Save box closed]- */
                    save(tb_text, grid);
                    if (file_io_success) {
                        strcpy(fname,tb_text);
                        opened_file=true;
                        unsaved_work=false;
                        open_date=datefstr(tb_text);
                    } else {
                        ; /* display error message */
                    }
                    break;
                case WIN_OPEN_BOX:                                                             /** -2.2.3.2 [Open box closed]- */
                    save(tb_text, grid);
                    open(tb_text, grid);
                    if (file_io_success) {
                        strcpy(fname,tb_text);
                        opened_file=true;
                        open_date=datefstr(tb_text);
                        unsaved_work=false;
                    } else {
                        ; /* display error message */
                    }
                    break;
                }
                closed_window_id = 0;
            } else {                                                   /* --(2.2.4) Standard condition - no windows involved-- */
                if (e.type == Event::KeyPressed) {                                                          /* KEYBOARD EVENTS */
                    if (e.key.control) {
                        if (e.key.code == Keyboard::S) {                                    /** -2.2.4.1 [Ctrl+S combination]- */
                            if (!opened_file) {
                                wptr=init_window(WIN_SAVE_BOX,IntRect(24,100,240,96),"Save As...");
                                focus_component(init_window_comp(wptr, TEXT_BOX, IntRect(24,48,192,24),""));
                                window_just_opened = true;
                            } else {
                                if (save(fname,grid)) unsaved_work=false;
                            }
                        }
                        if (e.key.code == Keyboard::O) {                                    /** -2.2.4.2 [Ctrl+O combination]- */
                            wptr=init_window(WIN_OPEN_BOX,IntRect(24,100,240,96),"Open File");
                            focus_component(init_window_comp(wptr, TEXT_BOX, IntRect(24,48,192,24),""));
                            window_just_opened = true;
                        }
                        if (e.key.code == Keyboard::T) {                                    /** -2.2.4.3 [Ctrl+T combination]- */
                            if (open(today_string,grid)) {
                                strcpy(fname,today_string);
                                opened_file=true;
                                open_date=today;
                            } else {
                                save(today_string, grid);
                                if (file_io_success) {
                                    strcpy(fname,today_string);
                                    opened_file=true;
                                    unsaved_work=false;
                                    open_date=today;
                                }
                            }
                        }
                    } else {
                        if (e.key.code == Keyboard::F12) {                                             /** -2.2.4.4 [F12 key]- */
                            wptr=init_window(WIN_SAVE_BOX,IntRect(36,100,240,96),"Save As...");
                            focus_component(init_window_comp(wptr, TEXT_BOX, IntRect(24,48,192,24),""));
                            window_just_opened = true;
                        }
                        if (e.key.code == Keyboard::Left) {                                           /** -2.2.4.5 [Left key]- */
                            if (open_date) {
                                char dstr[11];
                                strfdate(dstr,open_date-1);
                                if (open(dstr,grid)) {
                                    strcpy(fname,dstr);
                                    open_date--;
                                    unsaved_work=false;
                                }
                            }
                        }
                        if (e.key.code == Keyboard::Right) {                                         /** -2.2.4.6 [Right key]- */
                            if (open_date) {
                                char dstr[11];
                                strfdate(dstr,open_date+1);
                                if (open(dstr,grid)) {
                                    strcpy(fname,dstr);
                                    open_date++;
                                    unsaved_work=false;
                                }
                            }
                        }
                    }
                }

                if (e.type == Event::MouseButtonPressed) {                                               /* MOUSE PRESS EVENTS */
                    if (e.mouseButton.button == Mouse::Left) {                               /** -2.2.4.7 [Left mouse button]- */
                        if (x<0||y<0||x>=MA_WIDTH+MID_SEPARATOR||y>=MA_HEIGHT) {
                            if (x==-MARGIN_LEFT) { /* left-click on leftmost square (toolbar area) */
                                if (y < -MARGIN_TOP+NUM_ICONS_MAIN) at_hand=y+MARGIN_TOP;
                                else if (-MARGIN_TOP+NUM_ICONS_MAIN+1 <= y&&y <= -MARGIN_TOP+NUM_ICONS_MAIN+9) {
                                    at_hand=CA_ZERO+(y-(-MARGIN_TOP+NUM_ICONS_MAIN));
                                }
                            }
                        } else if (PART_WIDTH <= x&&x < PART_WIDTH+MID_SEPARATOR) { /* left click in middle separator */
                            ;
                        } else { /* left-click inside main area */
                            if (PART_WIDTH <= x) x -= MID_SEPARATOR;
                            if (!ranging) {
                                if (grid[x][y]!=at_hand) if (opened_file) unsaved_work=true;
                                grid[x][y]=at_hand;
                            } else {
                                if (y<yb || (y==yb && x<xb));
                                else while (true) {
                                    if (grid[xb][yb]!=at_hand) if (opened_file) unsaved_work=true;
                                    grid[xb][yb]=at_hand;
                                    if (xb==x && yb==y) break;
                                    xb++;
                                    if (xb==PART_WIDTH) {
                                        xb=0;
                                        yb++;
                                    } else if (xb==MA_WIDTH) {
                                        xb=PART_WIDTH;
                                        yb++;
                                    }
                                }
                                ranging=false;
                            }
                        }
                    }
                    if (e.mouseButton.button == Mouse::Right) {                             /** -2.2.4.8 [Right mouse button]- */
                        if (x<0||y<0||x>=MA_WIDTH+MID_SEPARATOR||y>=MA_HEIGHT) { /* right-click outside main area */
                            ;
                        } else if (PART_WIDTH <= x&&x < PART_WIDTH+MID_SEPARATOR) { /* left click in middle separator */
                            ;
                        } else { /* right-click inside main area */
                            if (PART_WIDTH <= x) x -= MID_SEPARATOR;
                            if (!ranging) {
                                ranging=true;
                                xb=x;
                                yb=y;
                            } else {
                                if (x==xb && y==yb) {
                                    ranging=false;
                                } else {
                                    xb=x;
                                    yb=y;
                                }
                            }
                        }
                    }
                    if (e.mouseButton.button == Mouse::Middle) {                           /** -2.2.4.9 [Middle mouse button]- */
                        if (PART_WIDTH <= x&&x < PART_WIDTH+MID_SEPARATOR);
                        else if (!ranging) {
                            if (PART_WIDTH <= x) x -= MID_SEPARATOR;
                            if (grid[x][y]!=0) if (opened_file) unsaved_work=true;
                            grid[x][y]=0;
                        } else {
                            if (PART_WIDTH <= x) x -= MID_SEPARATOR;
                            if (y<yb || (y==yb && x<xb));
                            else while (true) {
                                if (grid[xb][yb]!=0) if (opened_file) unsaved_work=true;
                                grid[xb][yb]=0;
                                if (xb==x && yb==y) break;
                                xb++;
                                if (xb==PART_WIDTH) {
                                    xb=0;
                                    yb++;
                                } else if (xb==MA_WIDTH) {
                                    xb=PART_WIDTH;
                                    yb++;
                                }
                            }
                            ranging=false;
                        }
                    }
                }
            }
        }

        app.clear(Color::White);                                                             /** ==== [3] DRAWING SESSION ==== */

        app.draw(sframe);                                                                     /* ===(3.1) Frame and toolbar=== */
        app.draw(s_toolbar);
        app.draw(s_icaf);

        if (opened_file) {                                                                     /* ===(3.2) Active file name=== */
            if (unsaved_work) strcat(fname,"*");
            fname_txt.setString(fname);
            if (unsaved_work) fname[strlen(fname)-1]='\0';
            FloatRect fr = fname_txt.getLocalBounds();
            fname_txt.setPosition(WINDOW_WIDTH/2-(int)fr.width/2, 0);
            if (open_date) {
                if (open_date==today) fname_txt.setFillColor(Color::Blue);
                else fname_txt.setFillColor(Color(64,64,255,255));
            }
            app.draw(fname_txt);
            if (open_date) fname_txt.setFillColor(Color::Black);
        }

        strftime(datetime_string, 20, "%Y-%m-%d %X", pcurrent_tm);                         /* ===(3.3)Current date and time=== */
        datetime_txt.setString(datetime_string);
        app.draw(datetime_txt);

        if (at_hand < CA_ZERO) tbsp = at_hand;                                                /* ===(3.4) Toolbar selection=== */
        else if (CA_ZERO+1 <= at_hand&&at_hand <= CA_ZERO+9) tbsp = NUM_ICONS_MAIN+(at_hand-CA_ZERO);
        else tbsp = at_hand;
        s_tb_sel.setPosition(0,tbsp*ICON_SIZE);
        app.draw(s_tb_sel);

        /* initialize frequency arrays... */
        for (int i=0; i<NUM_ICONS_MAIN; i++) {
            freq_main[i]=0;
            freq_main2[i]=0;
        }
        for (int i=1; i<=9; i++) freq_ca[i]=0;

        int prev_tile_read=-1;                                                                       /* ===(3.5) Main table=== */
        int second_part_offset=0;
        for (int p=0; p<=1; p++) {
            for (int j=0; j<MA_HEIGHT; j++) {
                for (int i=0; i<PART_WIDTH; i++) {
                    int bgcolor_print=0;
                    int io=i+PART_WIDTH*p;
                    if (CA_ZERO+1 <= grid[io][j]&&grid[io][j] <= CA_ZERO+9) { /* handle custom activity */
                        freq_ca[grid[io][j]-CA_ZERO]++;
                        /* determine if first or last in cluster */
                        bool ca_prev,ca_next;
                        ca_prev=(grid[io][j]==prev_tile_read);
                        if (i==PART_WIDTH-1 && j==MA_HEIGHT-1) ca_next=false;
                        else if (i==PART_WIDTH-1 && j!=MA_HEIGHT-1) ca_next=(grid[io][j]==grid[PART_WIDTH*p][j+1]);
                        else ca_next=(grid[io][j]==grid[io+1][j]);

                        /* choose appropriate display tile */
                        dgrid[io][j]=(ca_prev?CA_ZERO:grid[io][j])+(ca_next?0:10);
                        if (ca_prev) bgcolor_print=grid[io][j]-CA_ZERO;
                    } else dgrid[io][j]=grid[io][j];
                    if (bgcolor_print) {
                        s_bgcolor.setTextureRect(IntRect(0,bgcolor_print*ICON_SIZE,ICON_SIZE,ICON_SIZE));
                        s_bgcolor.setPosition((MARGIN_LEFT+i+second_part_offset)*ICON_SIZE,(MARGIN_TOP+j)*ICON_SIZE);
                        app.draw(s_bgcolor);
                        bgcolor_print=0;
                    }

                    /* increment frequency */
                    if (grid[io][j] < NUM_ICONS_MAIN)
                        (p==0?freq_main:freq_main2)[grid[io][j]]++;

                    /* draw tile */
                    s.setTextureRect(ss_vert(dgrid[io][j]));
                    s.setPosition((MARGIN_LEFT+i+second_part_offset)*ICON_SIZE,(MARGIN_TOP+j)*ICON_SIZE);
                    app.draw(s);
                    prev_tile_read=grid[io][j];

                    /* mark passed and current time in planning part */
                    if (p==1) {
                        if (open_date<today ||(open_date==today&& j<hour) ||(j==hour&& i<minute/PART_DURATION)) {
                            s_passedx.setPosition(s.getPosition());
                            app.draw(s_passedx);
                        }
                    }
                    /* render "now" mark */
                    if (open_date==today && j==hour && i==minute/PART_DURATION) {
                        s_nowmark.setPosition(s.getPosition());
                        app.draw(s_nowmark);
                    }
                }
            }
            second_part_offset=PART_WIDTH+MID_SEPARATOR;
        }

        if (ranging) {                                                                     /* ===(3.6) Main table selection=== */
            if (xb >= PART_WIDTH) xb += MID_SEPARATOR;
            s_selection.setPosition((MARGIN_LEFT+xb)*ICON_SIZE,(MARGIN_TOP+yb)*ICON_SIZE);
            if (xb >= PART_WIDTH) xb -= MID_SEPARATOR;
            app.draw(s_selection);
        }

        for (int i=0; i<NUM_ICONS_MAIN; i++) {                                                     /* ===(3.7) Frequencies === */
            snprintf(temp_num_txt, 4, "%d", freq_main[i]);
            txt.setString(temp_num_txt);
            txt.setPosition(ICON_SIZE+4, ICON_SIZE*i+2);
            app.draw(txt);
        }
        for (int i=1; i<=9; i++) {
            snprintf(temp_num_txt, 4, "%d", freq_ca[i]);
            txt.setString(temp_num_txt);
            txt.setPosition(ICON_SIZE+4, ICON_SIZE*(NUM_ICONS_MAIN+i));
            app.draw(txt);
        }

                                                                                                      /* ===(3.8) Window(s)=== */
        if (active_window!=NULL) {
            app.draw(s_fu);
            draw_window(&app, wptr);
        }

        app.display();
    }
    return 0;
}
