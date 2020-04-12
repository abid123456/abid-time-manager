/* awindows.cpp - windows engine */

#include <SFML/Graphics.hpp>
#include "awindows.h"

using namespace sf;

window *active_window;
win_comp *active_comp;

void draw_window(RenderWindow *rw, window *wp)
{
    if (wp==NULL) return;

    Font font;
    font.loadFromFile("fonts/cruft.ttf");

    /* draw window frame */
    Texture t_wframe;
      t_wframe.loadFromFile("images/window_frame.png");
        Sprite s_wframe(t_wframe);

    int index;
    for (int y = wp->r.top; y < wp->r.top + wp->r.height; y+=ICON_SIZE) {
        for (int x = wp->r.left; x < wp->r.left + wp->r.width; x+=ICON_SIZE) {
            index=7;
            if (y < wp->r.top + wp->r.height - ICON_SIZE) index-=3;
            if (y == wp->r.top) index-=3;
            if (x < wp->r.left + wp->r.width - ICON_SIZE) index+=1;
            if (x == wp->r.left) index-=2;
            s_wframe.setTextureRect(ss_vert(index));
            s_wframe.setPosition(x,y);
            (*rw).draw(s_wframe);
        }
    }

    /* draw title text */
    Text title_txt((wp->title),font,WIN_FONT_SIZE);
    title_txt.setFillColor(Color::Black);
    FloatRect fr = title_txt.getLocalBounds();
    title_txt.setPosition(v2f_pos(wp->r)+Vector2f(wp->r.width/2-fr.width/2,0));
    (*rw).draw(title_txt);

    char tb_buffer[MAX_TEXT_LENGTH+2];
    /* render components */
    for (int i=0; i<wp->comp_count; i++) {
        if (!wp->pcomp[i]->visible) continue;
        switch (wp->pcomp[i]->comp_id) {
        case TEXT_BOX:
            /* draw rectangle */
            RectangleShape rs(v2f_size(wp->pcomp[i]->r_relative));
            rs.setFillColor(Color::White);
            rs.setOutlineColor(Color::Black);
            rs.setOutlineThickness(-1);
            rs.setPosition(v2f_pos(wp->r)+v2f_pos(wp->pcomp[i]->r_relative));
            (*rw).draw(rs);
            /* draw text */
            strcpy(tb_buffer, wp->pcomp[i]->text);
            if (((int)((float)clock()*FLASH_FREQUENCY*2.0/CLOCKS_PER_SEC))%2) {
                int len = strlen(tb_buffer);
                tb_buffer[len]='_';
                tb_buffer[len+1]='\0';
            }
            Text tb_txt(tb_buffer, font, WIN_FONT_SIZE);
            tb_txt.setFillColor(Color::Black);
            tb_txt.setPosition(v2f_pos(wp->r)+v2f_pos(wp->pcomp[i]->r_relative)+Vector2f(6,2));
            (*rw).draw(tb_txt);
            break;
        }
    }
}

window *init_window(int window_id, IntRect r, const char *title)
{
    window *wp = (window*) malloc(sizeof (window));
    wp->window_id=window_id;
    wp->r=r;
    strcpy(wp->title, title);
    for (int i=0; i<MAX_COMP_IN_WINDOW; i++) wp->pcomp[i]=NULL;
    wp->comp_count=0;
    return wp;
}

win_comp *init_window_comp(window *wp, int comp_id, IntRect r_relative)
{
    win_comp *wcp = (win_comp*) malloc(sizeof (win_comp));
    wp->pcomp[wp->comp_count++]=wcp;
    wcp->parent=wp;

    wcp->comp_id=comp_id;
    wcp->r_relative=r_relative;
    wcp->visible=true;
    return wcp;
}

win_comp *init_window_comp(window *wp, int comp_id, IntRect r_relative, const char *text)
{
    win_comp *wcp = init_window_comp(wp, comp_id, r_relative);
    strcpy(wcp->text, text);
    return wcp;
}

void destroy_window(window *wp)
{
    for (int i=0; i<MAX_COMP_IN_WINDOW; i++)
        if (wp->pcomp[i]!=NULL) free(wp->pcomp[i]);
    if (active_window==wp) {
        active_comp=NULL;
        active_window=NULL;
    }
    free(wp);
}

void focus_window(window *wp) { active_window=wp; }
void focus_component(win_comp *wcp) { active_comp=wcp; focus_window((window*)wcp->parent); }
