#include <iostream>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>


using namespace std;

int FieldWidth = 12;
int FieldHeight = 18;
wstring tetromino[7];
unsigned char *pField = nullptr;
int nScreenWidth = 120;
int nScreenHeight = 40;

int Rotate(int px, int py, int r){
  switch (r % 4) {
    case 0: return py * 4 + px;          //0 degrees
    case 1: return 12 + py - (px*4);    //90 degrees
    case 2: return 15 - (py * 4) - px;  //180 degrees
    case 3: return 3 - py + (px * 4);   //270 degrees
  }
  return 0;
}

int CurrentPiece = 0 ;
int Currentrotation = 0;
int currentx = FieldWidth/2;
int currenty = 0;

bool doesitfit( int ntetromino, int rotation, int posx, int posy){
  for(int px = 0;px < 4; px++){
    for(int py = 0; py < 4; py++){
      //index of the tetromino 4*4 matrix
      int pi = Rotate(px,py,rotation);
      //index of the matrix into the game field
      int fi = (posy + py) * FieldWidth + (posx + px);

      if(posx + px >= 0 && posx + px < FieldWidth){
        if(posy + py >= 0 && posy + py < FieldHeight){
          if(tetromino[ntetromino][pi] == L'X' && pField[fi] != 0){
            return false; //Piece does not fit
          }
        }
      }

    }
  }
  return true;
}
int main(){
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"....");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L"....");

    tetromino[2].append(L"....");
    tetromino[2].append(L"..XX");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L"XX..");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"....");

    tetromino[5].append(L".X..");
    tetromino[5].append(L".X..");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"....");

    tetromino[6].append(L"....");
    tetromino[6].append(L"XXX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L"....");

    pField = new unsigned char[FieldWidth*FieldHeight];
    for(int x = 0; x < FieldWidth; x++)
      for(int y = 0; y < FieldHeight; y++)
        pField[y*FieldWidth + x] = (x == 0 || x == FieldWidth -1 || y == FieldHeight - 1) ? 9 : 0; //generamos la matriz del tablero y queremos que todos los espacios

    wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
    for(int i = 0; i < nScreenWidth*nScreenHeight;i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWriten = 0;

    bool GameOver = false;
    bool bKey[4];
    bool rotatehold;
    int speed = 20;
    int speedcounter = 0;
    bool forcedown;
    vector<int> vlines;

    while(!GameOver){
        //Timing
        this_thread::sleep_for(50ms);
        speedcounter++;
        forcedown = (speedcounter == speed);
        //WriteConsoleOutputCharacterW
        for(int k = 0; k < 4; k++){                         // R  L   D  Z
          bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }
        //Logic
        currentx += (bKey[0] && doesitfit(CurrentPiece,Currentrotation,currentx+1,currenty)) ? 1:0;
        currentx -= (bKey[1] && doesitfit(CurrentPiece,Currentrotation,currentx-1,currenty)) ? 1:0;
        currenty += (bKey[2] && doesitfit(CurrentPiece,Currentrotation,currentx,currenty+1)) ? 1:0;
        if(bKey[3]){
          Currentrotation += (!rotatehold && bKey[3] && doesitfit(CurrentPiece,Currentrotation+1,currentx,currenty)) ? 1:0;
          rotatehold = true;
        }
        else{
          rotatehold = false;
        }

        if(forcedown){
          if(doesitfit(CurrentPiece,Currentrotation,currentx,currenty+1)){
            currenty++;
          }
          else{
            //lock the falling piece in the field
            for(int px = 0; px < 4; px++){
              for(int py = 0; py < 4; py++){
                if(tetromino[CurrentPiece][Rotate(px,py, Currentrotation)] == L'X'){
                  pField[(currenty + py)*FieldWidth + (currentx + px)] = CurrentPiece + 1;
                }
              }
            }
            //check if there are any complete lines in the field
            for(int y =0;y<4;y++){
              if(currenty + y < FieldHeight-1){
                bool line = true;
                for(int x = 1; x < FieldWidth-1;x++){
                  line &= (pField[(currenty+y)*FieldWidth+x]) != 0;
                }
                if(line){
                  for(int px = 1; px < FieldWidth -1;px++){
                    pField[(currenty+y)*FieldWidth+px] = 8;
                  }
                  vlines.push_back(currenty+y);
                }


              }
            }
            //choose next piece
            currentx = FieldWidth/2;
            currenty = 0;
            Currentrotation = 0;
            CurrentPiece = rand() % 7;
            //piece does not fit
            GameOver = !doesitfit(CurrentPiece,Currentrotation,currentx,currenty);
          }
          speedcounter = 0;
        }

      for(int x = 0; x < FieldWidth; x++){
        for(int y = 0; y < FieldHeight; y++){
          screen[(y+2)*nScreenWidth + (x+2)] = L" ABCDEFG=#"[pField[y*FieldWidth + x]];
        }
      }

      for(int px = 0; px < 4; px++){
        for(int py = 0; py < 4; py++){
          if(tetromino[CurrentPiece][Rotate(px,py, Currentrotation)] == L'X'){
            screen[(currenty + py + 2)*nScreenWidth + (currentx + px +2)] = CurrentPiece + 65;
          }
        }
      }

      if(!vlines.empty()){
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWriten);
        this_thread::sleep_for(400ms);
        for(auto &v : vlines){
          for(int x = 1; x < FieldWidth - 1; x++){
            for(int y = v; y > 0; y--){
              pField[y*FieldWidth+x] = pField[(y-1)*FieldWidth + x];
            }
            pField[x]=0;
          }
        }
        vlines.clear();
      }

      WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWriten);


    }                                     //valgan 0 excepto cuando se trata del borde del tablero en dado caso le aisgnamos un 9 al espacio


};
