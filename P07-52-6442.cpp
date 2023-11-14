#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <string>


int windowWidth = 800;
int windowHeight = 800;
int stepSize = 10;
int originalStepSize = 10;  



//player
int playerX = 400;
int playerY = 400;
int playerXPrev = playerX;
int playerYPrev = playerY;
int playerRadius = 40;
int obstacleSize = 30;

float playerDirection = 0.0;
float playerSpeed = 5.0;
bool moving = false;


// makan el obstacles
int obstaclesX[] = {100, 300, 500, 700};
int obstaclesY[] = {600, 400, 500, 300};
int numObstacles = 4;
int obstacleSizeX = 30;
int obstacleSizeY = 10;


//collectables
int numCollectibles = 6;
int collectibleX[6];
int collectibleY[6];
bool collectibleCollected[6]; 
float collectibleRotationAngle = 0.0f; 



// speed powerup
int numPowerups = 1;
int powerupX[1];
int powerupY[1];
bool powerupCollected[1] = {false};
int powerUpTimer = 0;  
float powerUpRotationAngle = 0.0f;  
int powerupSizeX = 20; 
int powerupSizeY = 45;  


//obstacle size powerup
int numNewPowerups = 1;
int newPowerupX[1];
int newPowerupY[1];
bool newPowerupCollected[1] = {false};
int newPowerupTimer = 0; 
bool newPowerupActive = false;
float newpowerUpRotationAngle = 0.0f;  
const float newPowerupWidth = 30.0;  
const float newPowerupHeight = 20.0; 



//hagat el healthbar
int health = 100;       
int maxHealth = 100;
int healthBarWidth = 200;  
int healthBarHeight = 20;  
float healthBarX = (windowWidth - healthBarWidth) / 2;
float healthBarY = windowHeight - 30; 

int gameScore = 0;
int textX = 15;  
int textY = windowHeight - 30;  

//time
int elapsedSeconds = 0;
int gameDuration = 60; 
bool gameOver = false;


//hgat el goal
bool reachedGoal = false; 
float goalX = 60.0;
float goalY = 70.0;
float goalRadius = 10.0;
float goalRotationAngle = 0.0f;  



//el animation w el drawing bta3 el background square
float backgroundRotationAngle = 0.0f; 

void updateBackgroundRotation() {
    
    backgroundRotationAngle += 10.0f; 
    if (backgroundRotationAngle >= 360.0f) {
        backgroundRotationAngle -= 360.0f; 
    }
}

void drawCenteredSquare(float size, float backgroundRotationAngle) {
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0); // Yellow 
    glTranslatef(400.0, 400.0, 0.0); 
    glRotatef(backgroundRotationAngle, 0.0f, 0.0f, 1.0f); // rotation lel square
    glTranslatef(-size / 2.0, -size / 2.0, 0.0); 

    
    glBegin(GL_QUADS);
    glVertex2f(0.0, 0.0); 
    glVertex2f(size, 0.0); 
    glVertex2f(size, size); 
    glVertex2f(0.0, size); 
    glEnd();

    glPopMatrix();
}






void decreasePowerUpTimer(int value) {
    if (powerUpTimer > 0) {
        powerUpTimer--;
        glutTimerFunc(1000, decreasePowerUpTimer, 0);
    } else {
        // lw el powerup kheles
        stepSize = originalStepSize;
    }
}



void decreaseNewPowerupTimer(int value) {
    if (newPowerupTimer > 0) {
        newPowerupTimer--;
        glutTimerFunc(1000, decreaseNewPowerupTimer, 0);
    } else {
        
        numObstacles = 4; 
        newPowerupActive = false; 
    }
}




void timer(int value) {
    if (!gameOver) {
        elapsedSeconds++;
        glutPostRedisplay(); 
        glutTimerFunc(1000, timer, 0); 
    }
}



void drawTimer(int elapsedSeconds) {
    glColor3f(0.0f, 0.0f, 0.0f); 

    
    std::string timerText = "Game Time 1 min : " + std::to_string(elapsedSeconds) + "s";

    
    int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)timerText.c_str());
    
    
    glRasterPos2i(windowWidth - textWidth - 10, windowHeight - 30);
    for (char c : timerText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}





bool isCollidingWithObstacle(int x, int y);

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}



void initializeCollectibles() {
    srand(static_cast<unsigned>(time(nullptr)));
    const int minDistanceFromObstacle = 60;
    const int minDistanceFromBoundary = 60;

    for (int i = 0; i < numCollectibles; i++) {
        bool isObstacleCollision;
        bool isBoundaryCollision;
        
        do {
            //random positions lel collectibles
            collectibleX[i] = rand() % (windowWidth - 60) + 10;
            collectibleY[i] = rand() % (windowHeight - 60) + 10;

            
            isObstacleCollision = false;
            isBoundaryCollision = false;

            
            for (int j = 0; j < numObstacles; j++) {
                if (abs(collectibleX[i] - obstaclesX[j]) < obstacleSizeX / 2 + minDistanceFromObstacle &&
                    abs(collectibleY[i] - obstaclesY[j]) < obstacleSizeY / 2 + minDistanceFromObstacle) {
                    isObstacleCollision = true;
                    break;
                }
            }

            
            isBoundaryCollision = (collectibleX[i] < minDistanceFromBoundary || collectibleX[i] > windowWidth - minDistanceFromBoundary ||
                                  collectibleY[i] < minDistanceFromBoundary || collectibleY[i] > windowHeight - minDistanceFromBoundary);

        } while (isObstacleCollision || isBoundaryCollision);

        collectibleCollected[i] = false; 
    }
}




void initializePowerups() {
    srand(static_cast<unsigned>(time(nullptr)));
    const int minDistanceFromObstacle = 60;
    const int minDistanceFromBoundary = 60;

    for (int i = 0; i < numPowerups; i++) {
        bool isObstacleCollision;
        bool isBoundaryCollision;
        bool isCollectibleCollision;
        bool isOtherPowerupCollision;

        do {
            // Random positions for powerups
            powerupX[i] = rand() % (windowWidth - 60) + 10;
            powerupY[i] = rand() % (windowHeight - 60) + 10;

            isObstacleCollision = false;
            isBoundaryCollision = false;
            isCollectibleCollision = false;
            isOtherPowerupCollision = false;

            for (int j = 0; j < numObstacles; j++) {
                if (abs(powerupX[i] - obstaclesX[j]) < obstacleSizeX / 2 + minDistanceFromObstacle &&
                    abs(powerupY[i] - obstaclesY[j]) < obstacleSizeY / 2 + minDistanceFromObstacle) {
                    isObstacleCollision = true;
                    break;
                }
            }

            isBoundaryCollision = (powerupX[i] < minDistanceFromBoundary || powerupX[i] > windowWidth - minDistanceFromBoundary ||
                                  powerupY[i] < minDistanceFromBoundary || powerupY[i] > windowHeight - minDistanceFromBoundary);

            // Check for collectible collisions
            for (int j = 0; j < numCollectibles; j++) {
                if (abs(powerupX[i] - collectibleX[j]) < powerupSizeX / 2 + minDistanceFromObstacle &&
                    abs(powerupY[i] - collectibleY[j]) < powerupSizeY / 2 + minDistanceFromObstacle) {
                    isCollectibleCollision = true;
                    break;
                }
            }

            // Check for powerup collisions
            for (int j = 0; j < i; j++) {
                if (abs(powerupX[i] - powerupX[j]) < powerupSizeX / 2 + powerupSizeX / 2 &&
                    abs(powerupY[i] - powerupY[j]) < powerupSizeY / 2 + powerupSizeY / 2) {
                    isOtherPowerupCollision = true;
                    break;
                }
            }

        } while (isObstacleCollision || isBoundaryCollision || isCollectibleCollision || isOtherPowerupCollision);

        powerupCollected[i] = false;
    }
}



void initializeNewPowerups() {
    srand(static_cast<unsigned>(time(nullptr)));
    const int minDistanceFromObstacle = 60;
    const int minDistanceFromBoundary = 60;

    for (int i = 0; i < numNewPowerups; i++) {
        bool isObstacleCollision;
        bool isBoundaryCollision;
        bool isCollectibleCollision;
        bool isPowerupCollision;
        bool isOtherNewPowerupCollision;

        do {
            // Random positions for new powerups
            newPowerupX[i] = rand() % (windowWidth - 70) + 10;
            newPowerupY[i] = rand() % (windowHeight - 70) + 10;

            isObstacleCollision = false;
            isBoundaryCollision = false;
            isCollectibleCollision = false;
            isPowerupCollision = false;
            isOtherNewPowerupCollision = false;

            for (int j = 0; j < numObstacles; j++) {
                if (abs(newPowerupX[i] - obstaclesX[j]) < obstacleSizeX / 2 + minDistanceFromObstacle &&
                    abs(newPowerupY[i] - obstaclesY[j]) < obstacleSizeY / 2 + minDistanceFromObstacle) {
                    isObstacleCollision = true;
                    break;
                }
            }

            isBoundaryCollision = (newPowerupX[i] < minDistanceFromBoundary || newPowerupX[i] > windowWidth - minDistanceFromBoundary ||
                                  newPowerupY[i] < minDistanceFromBoundary || newPowerupY[i] > windowHeight - minDistanceFromBoundary);

            // Check for collectible collisions
            for (int j = 0; j < numCollectibles; j++) {
                if (abs(newPowerupX[i] - collectibleX[j]) < powerupSizeX / 2 + minDistanceFromObstacle &&
                    abs(newPowerupY[i] - collectibleY[j]) < powerupSizeY / 2 + minDistanceFromObstacle) {
                    isCollectibleCollision = true;
                    break;
                }
            }

            // Check for powerup collisions
            for (int j = 0; j < numPowerups; j++) {
                if (abs(newPowerupX[i] - powerupX[j]) < powerupSizeX / 2 + powerupSizeX / 2 &&
                    abs(newPowerupY[i] - powerupY[j]) < powerupSizeY / 2 + powerupSizeY / 2) {
                    isPowerupCollision = true;
                    break;
                }
            }

            // Check for other new powerup collisions
            for (int j = 0; j < i; j++) {
                if (abs(newPowerupX[i] - newPowerupX[j]) < powerupSizeX / 2 + powerupSizeX / 2 &&
                    abs(newPowerupY[i] - newPowerupY[j]) < powerupSizeY / 2 + powerupSizeY / 2) {
                    isOtherNewPowerupCollision = true;
                    break;
                }
            }

        } while (isObstacleCollision || isBoundaryCollision || isCollectibleCollision || isPowerupCollision || isOtherNewPowerupCollision);

        newPowerupCollected[i] = false;
    }
}




void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}


void drawPlayer() {
    
    glTranslatef(playerX, playerY, 0);
    glRotatef(playerDirection, 0, 0, 1);
    glTranslatef(-playerX, -playerY, 0);

    
    float scale = 0.5;

    // Body (square)
    glColor3f(0.0, 1.0, 0.0);
    glLineWidth(5.0);
    float halfSize = playerRadius * 0.3 * scale;
    glBegin(GL_QUADS);
    glVertex2f(playerX - halfSize, playerY + halfSize);
    glVertex2f(playerX + halfSize, playerY + halfSize);
    glVertex2f(playerX + halfSize, playerY - halfSize);
    glVertex2f(playerX - halfSize, playerY - halfSize);
    glEnd();

    // Arms (horizontal lines)
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glVertex2f(playerX, playerY + playerRadius * 0.2 * scale);
    glVertex2f(playerX + playerRadius * 0.5 * scale, playerY);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(playerX, playerY + playerRadius * 0.2 * scale);
    glVertex2f(playerX - playerRadius * 0.5 * scale, playerY);
    glEnd();

    // Legs (horizontal lines)
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glVertex2f(playerX, playerY - playerRadius * 0.5 * scale);
    glVertex2f(playerX + playerRadius * 0.3 * scale, playerY - playerRadius * scale);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(playerX, playerY - playerRadius * 0.5 * scale);
    glVertex2f(playerX - playerRadius * 0.3 * scale, playerY - playerRadius * scale);
    glEnd();

    // Circle (head)
    glColor3f(1.0, 0.0, 0.0);
    drawCircle(playerX, playerY + 11, playerRadius * 0.2 * scale, 50);

    // Hat (triangle)
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2f(playerX - playerRadius * 0.2 * scale, playerY + playerRadius * 0.8 * scale);
    glVertex2f(playerX + playerRadius * 0.2 * scale, playerY + playerRadius * 0.8 * scale);
    glVertex2f(playerX, playerY + playerRadius * scale);
    glEnd();
}



void drawHealthBar() {
    float healthPercentage = static_cast<float>(health) / static_cast<float>(maxHealth);
    float healthBarWidth = 200;  
    float healthBarX = (windowWidth - healthBarWidth) / 2; 

    
    float healthBarCurrentWidth = healthBarWidth * healthPercentage;

    
    glColor3f(1.0, 0.0, 0.0);  // Red color 
    glBegin(GL_QUADS);
    glVertex2f(healthBarX, healthBarY);  
    glVertex2f(healthBarX + healthBarWidth, healthBarY);  
    glVertex2f(healthBarX + healthBarWidth, healthBarY - healthBarHeight);  
    glVertex2f(healthBarX, healthBarY - healthBarHeight);  
    glEnd();

    
    glColor3f(0.0, 1.0, 0.0);  // Green color 
    glBegin(GL_QUADS);
    glVertex2f(healthBarX, healthBarY);  
    glVertex2f(healthBarX + healthBarCurrentWidth, healthBarY);  
    glVertex2f(healthBarX + healthBarCurrentWidth, healthBarY - healthBarHeight);  
    glVertex2f(healthBarX, healthBarY - healthBarHeight);  
    glEnd();
}







void specialKeys(int key, int x, int y) {
    int leftBoundary = playerRadius;
    int rightBoundary = windowWidth - playerRadius;
    int topBoundary = playerRadius;
    int bottomBoundary = windowHeight - playerRadius;
    

    playerXPrev = playerX;
    playerYPrev = playerY;

    if (gameOver) {
        return; 
    }
    switch (key) {
        case GLUT_KEY_UP:
            if (playerY < bottomBoundary - stepSize && !isCollidingWithObstacle(playerX, playerY + stepSize)) {
                playerY += stepSize;
                playerDirection = 0.0;
            } else {
                // Decrease health when hitting window boundary
                health -= 18;
            }
            break;
        case GLUT_KEY_DOWN:
            if (playerY > topBoundary + stepSize && !isCollidingWithObstacle(playerX, playerY - stepSize)) {
                playerY -= stepSize;
                playerDirection = 180.0;
            } else {
                
                health -= 18;
            }
            break;
        case GLUT_KEY_LEFT:
            if (playerX > leftBoundary + stepSize && !isCollidingWithObstacle(playerX - stepSize, playerY)) {
                playerX -= stepSize;
                playerDirection = 90.0;
            } else {
                
                health -= 18;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (playerX < rightBoundary - stepSize && !isCollidingWithObstacle(playerX + stepSize, playerY)) {
                playerX += stepSize;
                playerDirection = -90.0;
            } else {
              
                health -= 18;
            }
            break;
    }

    if (health < 0) {
        health = 0;  
    }

    moving = true;
    glutPostRedisplay();
}


bool isCollidingWithCollectible(int x, int y) {
    for (int i = 0; i < numCollectibles; i++) {
        if (!collectibleCollected[i] && abs(x - collectibleX[i]) < 20 && abs(y - collectibleY[i]) < 20) {
           
            collectibleCollected[i] = true;
            gameScore++;
           
            
            return true;
        }
    }
    return false;
}


bool isCollidingWithPowerup(int x, int y) {
    for (int i = 0; i < numPowerups; i++) {
        if (!powerupCollected[i] && abs(x - powerupX[i]) < 20 && abs(y - powerupY[i]) < 20) {
           
            powerupCollected[i] = true;

            return true;
        }
    }
    return false;
}


bool isCollidingWithNewPowerup(int x, int y) {
    for (int i = 0; i < numNewPowerups; i++) {
        if (!newPowerupCollected[i] && abs(x - newPowerupX[i]) < 20 && abs(y - newPowerupY[i]) < 20) {
            
            newPowerupCollected[i] = true;
            newPowerupActive = true;
            
          
            newPowerupTimer = 5;
            glutTimerFunc(1000, decreaseNewPowerupTimer, 0);
            
            return true;
        }
    }
    return false;
}



bool isCollidingWithObstacle(int x, int y) {
    for (int i = 0; i < numObstacles; i++) {
        int obstacleX = obstaclesX[i];
        int obstacleY = obstaclesY[i];

        
        if (x + playerRadius - 25 > obstacleX - obstacleSizeX / 2 &&
            x - playerRadius + 30 < obstacleX + obstacleSizeX / 2 &&
            y + playerRadius - 5 > obstacleY - obstacleSizeY &&
            y - playerRadius + 10 < obstacleY) {
            health -= 5;
            if (health < 0) {
                health = 0;  
            }
            return true;
        }
    }
    return false;
}



bool isCollidingWithGoal(int x, int y) {
    return (sqrt((x - goalX) * (x - goalX) + (y - goalY) * (y - goalY)) <= goalRadius);
}


void drawGoalArea() {
    
    glPushMatrix();

    
    glTranslatef(0.0, 10.0, 0.0);

    
    glTranslatef(60.0, 60.0, 0.0); 
    glRotatef(goalRotationAngle, 0, 0, 1); 
    glTranslatef(-60.0, -60.0, 0.0); 

   
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_TRIANGLES);
    glVertex2f(50.0, 50.0);       
    glVertex2f(70.0, 50.0);      
    glVertex2f(60.0, 70.0);       
    glEnd();

    // triangle
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_LINES);
    glVertex2f(60.0, 70.0); 
    glVertex2f(60.0, 80.0); 
    glEnd();

    // circle with lines
    glColor3f(0.0, 0.0, 0.0); // Black color
    drawCircle(60.0, 40.0, 10.0, 50); 

    //  two rectangles 
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_QUADS);
    // Left rectangle
    glVertex2f(50.0, 0.0);        
    glVertex2f(55.0, 0.0);        
    glVertex2f(55.0, 50.0);       
    glVertex2f(50.0, 50.0);      

    // Right rectangle
    glVertex2f(70.0, 0.0);        
    glVertex2f(75.0, 0.0);        
    glVertex2f(75.0, 50.0);       
    glVertex2f(70.0, 50.0);       
    glEnd();

    // square 
    glColor3f(0.0, 1.0, 0.0); 
    glBegin(GL_QUADS);
    glVertex2f(50.0, 0.0);        
    glVertex2f(75.0, 0.0);        
    glVertex2f(75.0, 25.0);       
    glVertex2f(50.0, 25.0);       
    glEnd();

    
    glPopMatrix();

   
    goalRotationAngle += 20.0; 
    if (goalRotationAngle >= 360.0) {
        goalRotationAngle -= 360.0;
    }
}




void drawObstacle(float x, float y, float size, float lineLength) {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x - size / 2, y - size);
    glVertex2f(x + size / 2, y - size);
    glEnd();

    glLineWidth(5.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x, y + lineLength);
    glEnd();
}





void drawCollectible(int x, int y, int index, float rotationAngle) {
   
    if (!collectibleCollected[index]) {
        glPushMatrix();  
        glTranslatef(x, y, 0);  
        glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);  
        glTranslatef(-x, -y, 0);  
        
        
        glColor3f(0.5, 0.5, 0.5);
        glBegin(GL_QUADS);
        glVertex2f(x - 10, y - 10);
        glVertex2f(x + 10, y - 10);
        glVertex2f(x + 10, y + 10);
        glVertex2f(x - 10, y + 10);
        glEnd();

        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 10, y + 10);
        glVertex2f(x + 10, y + 10);
        glVertex2f(x, y + 20);
        glEnd();

        glColor3f(0.5, 0.5, 0.5);
        glPointSize(3.0);
        glBegin(GL_POINTS);
        glVertex2f(x, y + 25);
        glEnd();

        glPopMatrix();  
    }
}





void drawPowerup(int x, int y, int index, float powerUpRotationAngle) {
   
    if (!powerupCollected[index]) {
        glPushMatrix(); 
        glTranslatef(x, y, 0); 
        glRotatef( powerUpRotationAngle, 0, 0, 1); // Rotate 
        
        //Line
        glColor3f(1.0, 0.0, 0.0); // Red color
        glLineWidth(5.0);
        glBegin(GL_LINES);
        glVertex2f(-15, 0);
        glVertex2f(15, 0);
        glEnd();

        // Rectangle
        glColor3f(0.0, 0.0, 1.0); // Blue color
        glBegin(GL_QUADS);
        glVertex2f(-10, -15);
        glVertex2f(10, -15);
        glVertex2f(10, 15);
        glVertex2f(-10, 15);
        glEnd();

        // Triangle
        glColor3f(0.0, 1.0, 0.0); // Green color
        glBegin(GL_TRIANGLES);
        glVertex2f(-10, 15);
        glVertex2f(10, 15);
        glVertex2f(0, 25);
        glEnd();

        // Circle 
        glColor3f(1.0, 0.0, 0.0); // Red color
        drawCircle(0, 30, 10.0, 50); 
        
        glPopMatrix(); 
    }
}





void drawNewPowerup(int x, int y, int index) {
    
    if (!newPowerupCollected[index]) {
        glPushMatrix(); 
        glTranslatef(x, y, 0); 
       
        float rotationAngle = fmodf(newpowerUpRotationAngle, 360.0f); 

        // Rotate 
        glRotatef(rotationAngle, 0, 0, 1);

        
        glColor3f(1.0, 0.0, 0.0); // Red 

        //rectangle
        glBegin(GL_QUADS);
        glVertex2f(-newPowerupWidth / 2, -newPowerupHeight / 2);
        glVertex2f(newPowerupWidth / 2, -newPowerupHeight / 2);
        glVertex2f(newPowerupWidth / 2, newPowerupHeight / 2);
        glVertex2f(-newPowerupWidth / 2, newPowerupHeight / 2);
        glEnd();

        //line
        glLineWidth(3.0);
        glBegin(GL_LINES);
        glVertex2f(0, -newPowerupHeight / 2 - 10);
        glVertex2f(0, newPowerupHeight / 2 + 10);
        glEnd();

        //circle
        glColor3f(0.0, 0.0, 1.0); // Blue 
        drawCircle(0, newPowerupHeight / 2 + 15, 10.0, 50); 

        //triangle
        glColor3f(0.0, 1.0, 0.0); // Green 
        glBegin(GL_TRIANGLES);
        glVertex2f(-5, newPowerupHeight / 2 + 15);
        glVertex2f(5, newPowerupHeight / 2 + 15);
        glVertex2f(0, newPowerupHeight / 2 + 25);
        glEnd();

        glPopMatrix(); 
    }
}






void drawBoundaryDecorations() {
    int numSegments = 10; 
    float segmentLength = 800.0 / numSegments; 

    // sequence of lines and points 
    float startX = 790.0;
    for (int i = 0; i < numSegments; i++) {
        if (i % 2 == 0) {
            glColor3f(0.0, 0.0, 0.0);  
            glLineWidth(2.0);           
            glBegin(GL_LINES);
            glVertex2f(startX, i * segmentLength);
            glVertex2f(startX, (i + 1) * segmentLength);
            glEnd();
        } else {
            glColor3f(0.0, 0.0, 0.0);  
            glPointSize(3.0);            
            glBegin(GL_POINTS);
            glVertex2f(startX - 10.0, (i + 1) * segmentLength); 
            glEnd();
        }
    }

     
    startX = 10.0;
    for (int i = 0; i < numSegments; i++) {
        if (i % 2 == 0) {
            glColor3f(0.0, 0.0, 0.0);  
            glLineWidth(2.0);          
            glBegin(GL_LINES);
            glVertex2f(startX, i * segmentLength);
            glVertex2f(startX, (i + 1) * segmentLength);
            glEnd();
        } else {
            glColor3f(0.0, 0.0, 0.0);  
            glPointSize(3.0);            
            glBegin(GL_POINTS);
            glVertex2f(startX + 10, (i + 1) * segmentLength); 
            glEnd();
        }
    }

    
    float startY = 790.0;
    for (int i = 0; i < numSegments; i++) {
        if (i % 2 == 0) {
            glColor3f(0.0, 0.0, 0.0);  
            glLineWidth(2.0);           
            glBegin(GL_LINES);
            glVertex2f(i * segmentLength, startY);
            glVertex2f((i + 1) * segmentLength, startY);
            glEnd();
        } else {
            glColor3f(0.0, 0.0, 0.0);  
            glPointSize(3.0);            
            glBegin(GL_POINTS);
            glVertex2f((i + 1) * segmentLength, startY - 10.0); 
            glEnd();
        }
    }

    
    startY = 10.0;
    for (int i = 0; i < numSegments; i++) {
        if (i % 2 == 0) {
            glColor3f(0.0, 0.0, 0.0);  
            glLineWidth(2.0);           
            glBegin(GL_LINES);
            glVertex2f(i * segmentLength, startY);
            glVertex2f((i + 1) * segmentLength, startY);
            glEnd();
        } else {
            glColor3f(0.0, 0.0, 0.0);  
            glPointSize(3.0);            
            glBegin(GL_POINTS);
            glVertex2f((i + 1) * segmentLength, startY + 10.0); 
            glEnd();
        }
    }
}



bool isHealthDepleted() {
    return health <= 0;
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCenteredSquare(400.0f, backgroundRotationAngle);
    updateBackgroundRotation();
    glPushMatrix();
    drawPlayer();
    glPopMatrix();


    

    for (int i = 0; i < numObstacles; i++) {
        drawObstacle(obstaclesX[i], obstaclesY[i], obstacleSizeX, obstacleSizeY);
    }

    

    for (int i = 0; i < numCollectibles; i++) {
        
        collectibleRotationAngle += 20.0f;  
        drawCollectible(collectibleX[i], collectibleY[i], i, collectibleRotationAngle);
    }

    for (int i = 0; i < numPowerups; i++) {
        if (!powerupCollected[i]) {
            powerUpRotationAngle += 25.0f;
            drawPowerup(powerupX[i], powerupY[i], i, powerUpRotationAngle);
        }
    }

    drawGoalArea(); 


    drawBoundaryDecorations();
    drawHealthBar(); 
    glColor3f(0.0f, 0.0f, 0.0f); 
    glRasterPos2i(textX, textY);
    std::string scoreText = "Game Score: " + std::to_string(gameScore);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

   
    if (isCollidingWithCollectible(playerX, playerY)) {
    }
   

    if (newPowerupActive) {
         //numObstacles=0;
         obstacleSizeX = 20;
         obstacleSizeY = 10;
 
    } else {
          // for (int i = 0; i < numObstacles; i++) {
        // drawObstacle(obstaclesX[i], obstaclesY[i], obstacleSizeX, obstacleSizeY);
        //}
          obstacleSizeX = 30;
          obstacleSizeY = 10;
    }

    
    for (int i = 0; i < numNewPowerups; i++) {
        newpowerUpRotationAngle += 25;
        drawNewPowerup(newPowerupX[i], newPowerupY[i], i);
    }


    if (isCollidingWithNewPowerup(playerX, playerY)) {
        newPowerupActive = true;
        newPowerupTimer = 5;
        
        glutTimerFunc(1000, decreaseNewPowerupTimer, 0);
    }


     
    if (!powerupCollected[0]) {
        drawPowerup(powerupX[0], powerupY[0], 0, powerUpRotationAngle);
    }


    if (isCollidingWithPowerup(playerX, playerY)) {

        powerupCollected[0] = true;
        stepSize = 20;
        powerUpTimer = 5;

        glutTimerFunc(1000, decreasePowerUpTimer, 0);
    }


    drawTimer(elapsedSeconds); 
    
    
    if (isHealthDepleted()) {
            gameOver = true;
            glClearColor(0.0, 0.0, 0.0, 1.0); 
            glClear(GL_COLOR_BUFFER_BIT);

            glColor3f(1.0, 1.0, 1.0); //text white

            std::string resultText = "YOU DIED";
            int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)resultText.c_str());
            glRasterPos2i((windowWidth - textWidth) / 2, windowHeight / 2);
            for (char c : resultText) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
            }
            
            glutSpecialFunc(nullptr);
        }

    if (!gameOver && elapsedSeconds >= gameDuration) {

        gameOver = true;
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0, 1.0, 1.0); 

        if (gameScore < 5) {
            // Player lost
            std::string resultText = "GAME OVER YOU LOST WITH LOW SCORE";
            int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)resultText.c_str());
            glRasterPos2i((windowWidth - textWidth) / 2, windowHeight / 2);
            for (char c : resultText) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
            }
        } else {
            // Player won
            std::string resultText = "YOU LOST WITH HIGH SCORE";
            int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)resultText.c_str());
            glRasterPos2i((windowWidth - textWidth) / 2, windowHeight / 2);
            for (char c : resultText) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
            }
        }
    }
     if (isCollidingWithGoal(playerX, playerY)) {
        reachedGoal = true;
    }

    if (reachedGoal) {
        // Game is over
        glClearColor(0.0, 1.0, 0.0, 1.0); // background color green
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.0, 0.0, 0.0); 

        std::string resultText = "GOAL YOU DID IT!";
        int textWidth = glutBitmapLength(GLUT_BITMAP_9_BY_15, (const unsigned char*)resultText.c_str());
        glRasterPos2i((windowWidth - textWidth) / 2, windowHeight / 2);
        for (char c : resultText) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }
    




    glutSwapBuffers();
}


int main(int argc, char** argv) {
     
    glutInit(&argc, argv);
    glutTimerFunc(1000, timer, 0);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("MY GAME");

    


    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);

    initializeCollectibles();  
    initializePowerups(); 
    initializeNewPowerups();
    

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutMainLoop();

    return 0;
}


