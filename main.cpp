#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT + TOP_MARGIN);

void move_bullets(vector<Bullet> &bullets)
{
    // move all bullets
    for (unsigned int i = 0; i < bullets.size(); i++)
    {
        if (!bullets[i].nextStep(STEP_TIME))
        {
            bullets.erase(bullets.begin() + i);
        }
    }
}

int move_bubbles(vector<Bubble> &bubbles, vector<Bullet> &bullets, int &scores)
{
    // move all bubbles
    for (unsigned int i = 0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
        for (unsigned int j = 0; j < bullets.size(); j++)
        {
            if (!bullets[j].nextStep(STEP_TIME))
            {
                bullets.erase(bullets.begin() + j);
            }
            double a = (bullets[j].get_center_x() - bubbles[i].get_center_x()) * (bullets[j].get_center_x() - bubbles[i].get_center_x());
            double b = (bullets[j].get_center_y() - bubbles[i].get_center_y()) * (bullets[j].get_center_y() - bubbles[i].get_center_y());
            double c = (bubbles[i].get_radius()) * (bubbles[i].get_radius());
            if (a + b < c)
            {
                if (bubbles[i].get_radius() > BUBBLE_DEFAULT_RADIUS)
                {
                    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius() / 2, -BUBBLE_DEFAULT_VX, BUBBLE_DEFAULT_VY, COLOR(255, 105, 180)));
                    bubbles.push_back(Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius() / 2, BUBBLE_DEFAULT_VX, BUBBLE_DEFAULT_VY, COLOR(255, 105, 180)));
                }
                bubbles.erase(bubbles.begin() + i);
                bullets.erase(bullets.begin() + j);
                scores++;
            }
        }
    }
    return scores;
}

vector<Bubble> create_bubbles(int level)
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X / 2.0, BUBBLE_START_Y, level * BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, BUBBLE_DEFAULT_VY, COLOR(80 * level, 70 * level, 50 * level)));
    bubbles.push_back(Bubble(WINDOW_X / 2.0, BUBBLE_START_Y, level * BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, BUBBLE_DEFAULT_VY, COLOR(80 * level, 70 * level, 50 * level)));
    return bubbles;
}

int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    string levelup("Level:1");
    Text levels(LEFT_MARGIN + (WINDOW_X / 3), BOTTOM_MARGIN / 2, levelup);

    string timer("Time:00/50");
    Text time(LEFT_MARGIN, TOP_MARGIN, timer);

    int HEALTH = 3;
    string msg("HEALTH:3/3");
    Text healthstatus(LEFT_MARGIN + WINDOW_X / 2, BOTTOM_MARGIN, msg);

    string score("Score:00");
    Text scored(LEFT_MARGIN + (WINDOW_X / 3), TOP_MARGIN, score);
    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);
    int level = 1;
    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles(level);

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;
    int i = 0;
    int j = 0;
    int scores = 0;
    // Main game loop
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if (c == 'a')
                shooter.move(STEP_TIME, true);
            else if (c == 'd')
                shooter.move(STEP_TIME, false);
            else if (c == 'w')
                bullets.push_back(shooter.shoot());
            else if (c == 'q')
                return 0;
        }
        // shooter bubble interaction
        for (unsigned int i = 0; i < bubbles.size(); i++)
        {
            if (abs(shooter.get_body_center_x() - bubbles[i].get_center_x()) < (bubbles[i].get_radius() + shooter.get_body_width() / 2))
            {
                if (abs(shooter.get_body_center_y() - bubbles[i].get_center_y()) < (bubbles[i].get_radius() + shooter.get_body_height()))
                {
                    HEALTH = HEALTH - 1;
                    msg[msg.length() - 3]--;
                    healthstatus.setMessage(msg);
                    bubbles[i].nextStep(0.3);
                    break;
                }
            }
        }
        if (HEALTH == 0)
        {
            Text cha(LEFT_MARGIN + (WINDOW_X / 3), BOTTOM_MARGIN / 2, "GAME OVER");
            wait(5);
            break;
        }
        if (bubbles.size() == 0 && level < 3)
        {
            level++;
            bubbles = create_bubbles(level);
            levelup[levelup.length() - 1]++;
            levels.setMessage(levelup);
        }
        if (bubbles.size() == 0 && level == 3)
        {
            Text chara(LEFT_MARGIN + (WINDOW_X / 3), BOTTOM_MARGIN / 2, "CONGRATULATIONS");
            wait(5);
            break;
        }
        // Update the bubbles,bullets
        scores = move_bubbles(bubbles, bullets, scores);
        if (scores > 0)
        {
            score[score.length() - 1] = 48 + scores % 10;
            score[score.length() - 2] = 48 + scores / 10;
            scored.setMessage(score);
        }
        wait(STEP_TIME);
        i++;
        if (i == 50)
        {
            i = 0;
            j++;
            if (timer[timer.length() - 4] == '9')
            {
                timer[timer.length() - 4] = '0';
                timer[timer.length() - 5]++;
            }
            else
            {
                timer[timer.length() - 4]++;
            }
            time.setMessage(timer);
        }
        if (j == 50)
        {
            Text timeup(LEFT_MARGIN + (WINDOW_X / 3), BOTTOM_MARGIN / 2, "TIME UP");
            wait(5);
            Text gameover(LEFT_MARGIN + (WINDOW_X / 3), BOTTOM_MARGIN / 2, "GAME OVER");
            wait(5);
            break;
            break;
        }
    }
}
