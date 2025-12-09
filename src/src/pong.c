/**
 *  (c) 2025 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms, you may use Microchip software
 *  and any derivatives exclusively with Microchip products. You're responsible
 *  for complying with 3rd party license terms applicable to your use of 3rd
 *  party software (including open source software) that may accompany Microchip
 *  software.
 *
 *  SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 *  APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
 *  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
 *  THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 *  CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY,
 *  YOU PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file  pong.c
 *  @brief This file contains the needed functions for the Pong application.
 */

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include "../include/mainApp.h"

#define COURT_X             0U
#define COURT_Y             10U   // Move court up-down    
#define COURT_LENGTH_X      PANEL_XSIZE
#define COURT_LENGTH_Y      20U

#define PADDLE_HEIGHT       ((COURT_LENGTH_Y - 2U) / 2U)
#define ENEMY_X             (COURT_X + COURT_LENGTH_X - 3U)
#define PLAYER_X            (COURT_X + 2U)

#define PADDLE_COLOR        BLUE
#define PLAYER_PADDLE_COLOR GREEN

#define BALL_COLOR          RED
#define SCORE_COLOR         PURPLE

#define PADDLE_COUNTER      2U
#define BALL_COUNTER        1U

static int8_t enemy_direction;
static int8_t ball_direction[2];
static uint8_t ball_position[2];
static uint8_t enemy_y;
static uint8_t player_y;
static uint8_t half_paddle = PADDLE_HEIGHT / 2U;
static uint8_t ball_counter = BALL_COUNTER;
static uint8_t paddle_counter = PADDLE_COUNTER;
static uint8_t player_score = 0;
static uint8_t enemy_score = 0;

static bool start_game = false; 
static bool end_game = false; 
static bool init_round = false;

typedef enum {
    PLAYER = 0,
    ENEMY,
    INIT
} round_winner_t;

void PONG_ScoreUpdate(uint8_t winner)
{
    uint8_t x_pos;
    uint8_t score;
    
    if(winner == (uint8_t)PLAYER)
    {
       x_pos =  COURT_X + 1U;
       score = player_score;
    }
    else if(winner == (uint8_t)ENEMY)   
    {
       x_pos =  COURT_X + COURT_LENGTH_X - 3U*(FONT_XSIZE +1U) - 1U;
       score = enemy_score;
    }
    else return;
    
    WS2812_DrawColorSet(SCORE_COLOR);    
    WS2812_ValuePrint(x_pos, 1, score);
}

void PONG_RoundNew(void)
{
    start_game = false;
    init_round = true;
    
    WS2812_DrawColorSet(BLACK);
    WS2812_PixelSet(WS2812_PanelXYOffsetGet(ball_position[0], ball_position[1]));
    
    WS2812_DrawColorSet(PADDLE_COLOR);
    for(uint8_t i = enemy_y - half_paddle; i <= enemy_y + half_paddle; i++)
    {
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ENEMY_X, i)); 
    }
    WS2812_DrawColorSet(PLAYER_PADDLE_COLOR);
    
    for(uint8_t i = player_y - half_paddle; i <= player_y + half_paddle; i++)
    {
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(PLAYER_X, i)); 
    }
    
    ball_position[0] = (COURT_LENGTH_X + COURT_X) / 3U + (rand() % ((COURT_LENGTH_X + COURT_X) / 3U));
    ball_position[1] = COURT_Y + 1U + (rand() % (COURT_LENGTH_Y - 3U));

    ball_direction[0] = ((rand() & (0x0A)) < 5 ) ? -1 : 1;
    ball_direction[1] = -1 + (rand() & (0x02));
    
    WS2812_DrawColorSet(BALL_COLOR);
    WS2812_PixelSet(WS2812_PanelXYOffsetGet(ball_position[0], ball_position[1]));
    
    PONG_ScoreUpdate(ENEMY);
    PONG_ScoreUpdate(PLAYER);
}

void PONG_Initialize(void)
{   
    WS2812_MatrixClear(0);

    WS2812_DrawColorSet(WHITE);
    WS2812_RectangleDraw(COURT_X, COURT_Y, COURT_LENGTH_X, COURT_LENGTH_Y);
    
    enemy_y = COURT_Y + half_paddle + 1U;
    enemy_direction = 1;
    
    player_y = COURT_Y + half_paddle + 1U;
    
    player_score = 0;
    enemy_score = 0;
    
    PONG_RoundNew();
}

void PONG_EnemyPaddleMove(void)
{
    if( ((int8_t)enemy_y + (int8_t)half_paddle + enemy_direction == (int8_t)(COURT_LENGTH_Y + COURT_Y) - 1) || ((int8_t)enemy_y - (int8_t)half_paddle + enemy_direction == (int8_t)COURT_Y) )
    {
        enemy_direction = -1 * enemy_direction;
    }
    
    if(enemy_direction == 1)
    {
        WS2812_DrawColorSet(BLACK);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ENEMY_X, (uint16_t)(enemy_y - half_paddle)));
        WS2812_DrawColorSet(PADDLE_COLOR);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ENEMY_X, enemy_y + half_paddle + 1U));
        enemy_y++;
    }
    
    else
    {
        WS2812_DrawColorSet(BLACK);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ENEMY_X, (uint16_t)(enemy_y + half_paddle)));
        WS2812_DrawColorSet(PADDLE_COLOR);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ENEMY_X, enemy_y - half_paddle - 1U));
        enemy_y--;
    }
}

void PONG_PlayerPaddleMove(uint8_t rdKey)
{
    bool valid_move = false;
    
    if(rdKey == (uint8_t)DOWN && (player_y + half_paddle + 1U < (COURT_LENGTH_Y + COURT_Y) - 1U))
    {
        valid_move = true;
    }
    
    else if(rdKey == (uint8_t)UP && (player_y - half_paddle - 1U > COURT_Y))
    {
        valid_move = true;
    }
    
    if(valid_move == true)
    {
        if(rdKey == (uint8_t)DOWN)
        {
            WS2812_DrawColorSet(BLACK);
            WS2812_PixelSet(WS2812_PanelXYOffsetGet(PLAYER_X, (uint16_t)(player_y - half_paddle)));
            WS2812_DrawColorSet(PLAYER_PADDLE_COLOR);
            WS2812_PixelSet(WS2812_PanelXYOffsetGet(PLAYER_X, player_y + half_paddle + 1U));
            player_y++;
        }
        
        else
        {
            WS2812_DrawColorSet(BLACK);
            WS2812_PixelSet(WS2812_PanelXYOffsetGet(PLAYER_X,(uint16_t)(player_y + half_paddle)));
            WS2812_DrawColorSet(PLAYER_PADDLE_COLOR);
            WS2812_PixelSet(WS2812_PanelXYOffsetGet(PLAYER_X, player_y - half_paddle - 1U));
            player_y--;
        }
    }
}

void PONG_CollisionCheck_BallMove(void)
{
    {
        // Delete ball
        WS2812_DrawColorSet(BLACK);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ball_position[0], ball_position[1]));

        ball_position[0] = (int8_t)ball_position[0] + ball_direction[0];
    
        // Check collision with up and down walls
        if((int8_t)ball_position[1] + ball_direction[1] < (int8_t)COURT_Y + 1) ball_direction[1] = ball_direction[1] * -1;
        if((int8_t)ball_position[1] + ball_direction[1] > (int8_t)(COURT_LENGTH_Y + COURT_Y) - 2) ball_direction[1] = ball_direction[1] * -1;

        ball_position[1] = (int8_t)ball_position[1] + ball_direction[1];  

        // Draw ball in new position:
        WS2812_DrawColorSet(BALL_COLOR);
        WS2812_PixelSet(WS2812_PanelXYOffsetGet(ball_position[0], ball_position[1]));

        // Check for collision of the ball and the player:
        if( (ball_position[0]  ==(PLAYER_X + 1U)) && ( ( (ball_position[1] >= player_y - half_paddle) && (ball_position[1] <= player_y + half_paddle) )
                || ( (ball_position[1] == player_y - half_paddle - 1U) && (ball_direction[1] >= 0) ) || ( (ball_position[1] == player_y + half_paddle + 1U) && (ball_direction[1] <= 0) ) ) )
        {
            ball_direction[0] = 1;
            ball_direction[1] = -1 + (int8_t)((uint8_t)rand() & (uint8_t)(0x02));
        }

        // Check for collision of the ball and the enemy:
        if( (ball_position[0]  == (ENEMY_X - 1U)) && ( ( (ball_position[1] >= enemy_y - half_paddle) && (ball_position[1] <= enemy_y + half_paddle) ) ||
            ( (ball_position[1] == enemy_y - half_paddle - 1U) && (ball_direction[1] >= 0) ) || ( (ball_position[1] == enemy_y + half_paddle + 1U) && (ball_direction[1] <= 0) ) ) )
        {
            ball_direction[0] = -1;
            ball_direction[1] = -1 + (int8_t)((uint8_t)rand() & (uint8_t)(0x02));
        } 

        // Check for player loose:
        if(ball_position[0] == ENEMY_X)
        {
            player_score++;
            PONG_RoundNew();
        }

        // Check for enemy loose:
        if(ball_position[0] == PLAYER_X)
        {
            enemy_score++;
            PONG_RoundNew();            
        } 
    }
}

bool PONG_Main(uint8_t rdKey)
{
    while (WS2812_IsBusy() == true);
 
    switch (rdKey)
    {
        case INIT_APP:
            PONG_Initialize();
            return true;
            break;
        case MENU_SELECT:
            if (APP_StatusGet() == (uint8_t)APP_RUN) 
            {
                APP_StatusSet(APP_PAUSE);
            }
            else if (APP_StatusGet() == (uint8_t)APP_PAUSE) 
            {
                APP_StatusSet(APP_RUN);
            }
            return false;
        default:
           break;
    }
    
    if (APP_StatusGet() != (uint8_t)APP_RUN) return false;   
    
    if(init_round == true)
    {
        init_round = false;
        return true;
    }
    
    if(start_game == false && (rdKey == (uint8_t)UP || rdKey == (uint8_t)DOWN))
    {
        start_game = true;
    }
    
    if(start_game == true)
    {
        
        ball_counter--;
        
        if(ball_counter == 0U)
        {
            ball_counter = BALL_COUNTER;
            PONG_CollisionCheck_BallMove();
        }
        
        if(end_game == false && start_game == true)
        {
            paddle_counter--;
            
            PONG_PlayerPaddleMove(rdKey);
        
            if(paddle_counter == 0U)
            {
                paddle_counter = PADDLE_COUNTER;
                PONG_EnemyPaddleMove();
            }
        }
    }
    return start_game;
}