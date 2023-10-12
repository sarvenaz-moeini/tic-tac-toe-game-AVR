
#include <io.h>
#include <lcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#asm
    .equ __lcd_port=0x12
#endasm

// global variable

int game[3][3];
int win[8];
int player, count, state, r, c, k;
unsigned char row[4]={0xFE, 0xFD, 0xFB, 0xF7};  
char txt[20];
unsigned char num[2]={0x06, 0x5B};

void keypad()
{
    while(1)
    {
        for(r=0; r<4; r++)
        {
            c=4;
            PORTC=row[r];
            DDRC=0x0F;
            if(PINC.4 == 0)
                c=0;
            if(PINC.5 == 0)
                c=1;
            if(PINC.6 == 0)
                c=2;
            if(PINC.7 == 0)
                c=3;   
            if(!(c == 4))
            {
                while(PINC.4 == 0);
                while(PINC.5 == 0);
                while(PINC.6 == 0);
                while(PINC.7 == 0);
                return;
            }
            delay_ms(5);
        }
    }
}

void init()
{
    player=0;
    count=0;
    state=0;  
    strcpy(txt, "");    
    for(r=0; r<3; r++)
        for(c=0; c<3; c++)
            game[r][c]=0;                           // initialing game array
    for(r=0; r<8; r++)   
        win[r]=0;                                   // initialing win array  
    r=0;
    c=0;
    lcd_clear();
    lcd_puts("TicTacToe Game\nPress ON to Start...");
    do{
        keypad();
        k=r * 4 + c;
    } while(k != 12);
    lcd_clear();
    for(r=3; r>0; r--)
    {
        sprintf(txt, "Game Starts in:%d", r); 
        lcd_puts(txt);
        delay_ms(100);
        lcd_clear();
    }
    PORTA=num[player];
}

void showBoard()
{
    lcd_clear();
    for(r=0; r<3; r++)
        for(c=0; c<3; c++)
        {
            lcd_gotoxy(c, r);
            if(game[r][c] == 1)
                lcd_putchar('X');
            else if(game[r][c] == -1)
                lcd_putchar('O');
            else
                lcd_putchar('-');
        }
}

void winnerCheck()
{
    for(r=0; r<8; r++)
        win[r] = 0;
    for(r=0; r<3; r++)
        for(c=0; c<3; c++)
        {
            win[r]+=game[r][c]; 
            win[r+3]+=game[c][r];
            if(r == c)
                win[6]+=game[r][c];
            if(r+c == 2)
                win[7]+=game[r][c];
        }
}

void main(void)
{
    PORTC=0xFF;                         // initial value
    DDRC=0x0F;                          // input / output    
    PORTA=0x00;                        // initial value
    DDRA=0xFF;                          // output  
    lcd_init(20); 
    init();    
    showBoard();
    while(state == 0)
    {
        keypad();
        if(r != 3 && c != 3)
        {
            if(game[r][c] == 0)
            {
                if(player == 0)
                    game[r][c] = 1;
                else
                    game[r][c] = -1;
                player=!player; 
                PORTA=num[player];                                           
                count++;
            }
        }  
        k=r*4+c;
        if(k == 13)
            init();
        showBoard();
        winnerCheck();
        for(r=0; r<8; r++)
            if(win[r] == 3)
                state = 1;
            else if(win[r] == -3)
                state = -1;
        if(count == 9 && state == 0)
            state = 2;
    }  
    lcd_gotoxy(3,3);
    if(state == 1)
        lcd_puts("Player X wins!");
    else if(state == -1)
        lcd_puts("Player O wins!");
    else if(state == 2)
        lcd_puts("Its a Draw!!");
    while(1)
    {
        keypad();
        k=r*4+c;
        if(k == 13)
            init();
    }
}
