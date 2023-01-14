#define OLC_PGE_APPLICATION

#include <iostream>
#include <chrono>
#include <thread>
#include "olcPixelGameEngine.hpp"

olc::Pixel tetcolors[10] = {
        olc::RED,
        olc::MAGENTA,
        olc::YELLOW,
        olc::GREEN,
        olc::CYAN,
        olc::BLUE,
        olc::DARK_YELLOW,
        olc::DARK_RED,
        olc::GREY,
        olc::WHITE,
};

// Cyan Tetramino
std::wstring tetramino[7];

void define_tetraminos()
{
    tetramino[0].append(L"  X ");
    tetramino[0].append(L"  X ");
    tetramino[0].append(L"  X ");
    tetramino[0].append(L"  X ");

    tetramino[1].append(L"  X ");
    tetramino[1].append(L" XX ");
    tetramino[1].append(L" X  ");
    tetramino[1].append(L"    ");

    tetramino[2].append(L" X  ");
    tetramino[2].append(L" XX ");
    tetramino[2].append(L"  X ");
    tetramino[2].append(L"    ");

    tetramino[3].append(L"    ");
    tetramino[3].append(L" XX ");
    tetramino[3].append(L" XX ");
    tetramino[3].append(L"    ");

    tetramino[4].append(L"  X ");
    tetramino[4].append(L" XX ");
    tetramino[4].append(L"  X ");
    tetramino[4].append(L"    ");

    tetramino[5].append(L"    ");
    tetramino[5].append(L" XX ");
    tetramino[5].append(L"  X ");
    tetramino[5].append(L"  X ");

    tetramino[6].append(L"    ");
    tetramino[6].append(L" XX ");
    tetramino[6].append(L" X  ");
    tetramino[6].append(L" X  ");
}

int rotate(int px, int py, int r)
{
    switch(r%4)
    {
        case 0: return py * 4 + px; // 0 degrees
        case 1: return 12 + py - (px*4); // 90 degrees
        case 2: return 15 - (py * 4) - px; // 180 degrees
        case 3: return 3 - py + (px * 4); // 270 degrees
    }
    return 0;
}

int fieldWidth = 12;
int fieldHeight = 18;


unsigned char *pField = nullptr;
void setup_playing_field()
{
    pField = new unsigned char[fieldWidth*fieldHeight];
    for (int x = 0; x < fieldWidth; x++)
    {
        for (int y = 0; y < fieldHeight; y++)
        {
            pField[y*fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;
        }
    }
}

bool does_piece_fit(int nTetromino, int rotation, int posX, int posY)
{
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            int pi = rotate(px, py, rotation);

            int fi = (posY + py) * fieldWidth + (posX + px);

            if (posX + px >= 0 && posX + px < fieldWidth) {
                if (posY + py >= 0 && posY + py < fieldHeight) {
                    if (tetramino[nTetromino][pi] == L'X' && pField[fi] != 0)
                        return false;
                }
            }
        }
    }
    return true;
}



class Tetris : public olc::PixelGameEngine
{

public:
    int score = 0;

    float gameTime = 0;

    int currentPiece = 0;
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;

    float gamespeed = 1; // Increment to speed up gameticks

    bool gameOver = false;


    std::vector<int> vLines;

    Tetris()
    {
        sAppName = "Tetris in C++ by Josh";
        score = 0;
        ResetGameState();
        // Clear board
    }
    bool OnUserCreate() override
    {
        return true;
    }

    void ResetGameState()
    {
        score = 0;
        gameTime = 0;
        currentPiece = 0;
        currentRotation = 0;
        currentX = fieldWidth / 2;
        currentY = 0;
        float gamespeed = 1;
    }


    void DrawBoard()
    {
        for (int x = 0; x < fieldWidth; x++)
        {
            for (int y = 0; y < fieldHeight; y++)
            {
                auto cell = pField[y*fieldWidth + x];
                if (cell > 0)
                {
                    olc::Pixel color = tetcolors[cell];
                    olc::Pixel outline = color*0.5;
                    this->FillRect(x*4, y*4, 4, 4, color);
                    this->FillRect(x*4, y*4, 1, 4, outline);
                    this->FillRect(x*4, y*4, 4, 1, outline);
                }
            }
        }
    }

    void DrawCurrentPiece()
    {
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetramino[currentPiece][rotate(px, py, currentRotation)]==L'X')
                {

                    olc::Pixel color = tetcolors[currentPiece+1];
                    this->FillRect(
                            (currentX+px)*4,
                            (currentY+py)*4,
                            4, 4,
                            color);
                    olc::Pixel outline = color*0.5;
                    this->FillRect(
                            (currentX+px)*4,
                            (currentY+py)*4,
                            4, 1, outline);
                    this->FillRect(
                            (currentX+px)*4,
                            (currentY+py)*4,
                            1, 4, outline);
                }


    }

    void DrawInfo()
    {
        DrawString(0, 80, "Scr:"+ std::to_string(score));
    }

    void Input()
    {
        // left
        if (this->GetKey(olc::Key::A).bPressed &&
            does_piece_fit(currentPiece, currentRotation, currentX-1, currentY))
            currentX--;

        // right
        if (this->GetKey(olc::Key::D).bPressed &&
            does_piece_fit(currentPiece, currentRotation, currentX+1, currentY))
            currentX++;

        // Down
        if (this->GetKey(olc::Key::S).bPressed &&
            does_piece_fit(currentPiece, currentRotation, currentX, currentY+1))
            currentY++;

        // Rotate
        if (this->GetKey(olc::Key::Z).bPressed &&
            does_piece_fit(currentPiece, currentRotation+1, currentX, currentY))
            currentRotation++;
    }

    void Render()
    {
        DrawBoard();
        DrawCurrentPiece();
        DrawInfo();
    }

    // Checks for instances of "Tetris" (a full line) on the board
    void CheckForTetris()
    {
        // Check for horiz lines
        for (int py = 0; py < 4; py++)
            if (currentY + py < fieldHeight - 1)
            {
                bool bLine = true;
                for (int px = 1; px < fieldWidth-1; px++)
                {
                    bLine &= (pField[(currentY+py)*fieldWidth+px]) != 0;
                }

                if (bLine)
                {
                    for (int px = 1; px < fieldWidth-1; px++)
                    {
                        pField[(currentY+py) * fieldWidth + px] = 8;

                    }

                    vLines.push_back(currentY+py);
                }
            }
    }

    void ClearLines()
    {
        int prescore = 0;
        for (auto &v: vLines)
        {
            prescore++;
            for (int px = 1; px < fieldWidth-1; px++)
            {
                for (int py = v; py > 0; py--)
                {
                    pField[py*fieldWidth+px] = pField[(py-1)*fieldWidth+px];

                }
                pField[px] = 0;
            }
        }


        score += 50*prescore;
        vLines.clear();

    }

    void NextPiece()
    {
        // Choose Next Piece
        currentX = fieldWidth/2;
        currentY = 0;
        currentRotation = 0;
        currentPiece = rand() % 7;
    }
    void DropCurrentPiece() {
        // lock current piece into the field
        for (int px = 0; px < 4; px++)
        {
            for (int py = 0; py < 4; py++)
            {
                if (tetramino[currentPiece][rotate(px, py, currentRotation)]==L'X')
                    pField[(currentY+py)*fieldWidth + (currentX+px)] = currentPiece+1;
            }
        }
    }
    void GameTick()
    {
        // Can the piece move?
        if (does_piece_fit(currentPiece, currentRotation, currentX, currentY+1)) {
            currentY++;
        } else {
            DropCurrentPiece();
            CheckForTetris();
            NextPiece();
            gamespeed++;

            if (!vLines.empty())
            {
                DrawBoard(); // ASS HACK
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(400ms);
                ClearLines();
            }
        }

        // if piece does not fit
        gameOver = !does_piece_fit(currentPiece, currentRotation, currentX, currentY);
        if (gameOver)
        {

            DrawString(0, 0, "YOU LOSE FUCKFACE :(");
            // TODO: Reset
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);
            ResetGameState();
            return;
        }

    }

    bool OnUserUpdate(float delta) override
    {

        Clear(olc::BLACK);

        gameTime += delta;

        if (gameTime > 1/gamespeed)
        {
            gameTime = 0;
            GameTick();
        }
        Input();
        Render();
        return true;
    }
};

int main() {

    std::cout << "Spin it up!" << std::endl;
    define_tetraminos();
    setup_playing_field();

    Tetris game;
    if (game.Construct(92, 128, 4, 4))
        game.Start();
    return 0;
}
