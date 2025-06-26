#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <commdlg.h>
#include <map>
#include <algorithm>
#include "resource.h"
#define IDD_MYDIALOG 101
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

enum class Ksztalt { Kwadrat, Kolo, Trojkat };

bool odlozony = true;
struct Element {
    Ksztalt typ;
    int x, y;
    int rozmiar;
    int masa;
    bool czyPodniesiony = false;
};
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hDlg, 1);
            return TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, -1);
            return TRUE;
        }  
    }
    return FALSE;
}
std::vector<Element> elementy = {

};

Ksztalt dozwolonyKsztalt = Ksztalt::Kolo; // domyślnie kule
int dowolnamasa = 500; // domyślna masa
int ramieX = 500;
int ramieY = 100;
Element* podniesiony = nullptr;
Element* pomocny = podniesiony;
std::map<Element*, std::vector<Element*>> wieze;
const int wysokoscPodloza = 450;
ULONG_PTR token;
// Zmienne do animacji
bool automatyczneBudowanie = false;
int animacjaEtap = 0;
Element* animEl1 = nullptr;
Element* animEl2 = nullptr;
Element* animEl3 = nullptr;
void InicjalizujGDIPlus() {
    GdiplusStartupInput input;
    GdiplusStartup(&token, &input, NULL);
}

void ZakonczGDIPlus() {
    GdiplusShutdown(token);
}



void RysujElement(Graphics& g, const Element& e, bool aktywny = false) {
    Color kolor = aktywny ? Color(255, 255, 165, 0) :
        (e.typ == Ksztalt::Kwadrat ? Color(255, 255, 0, 0) :
            e.typ == Ksztalt::Kolo ? Color(255, 0, 0, 255) :
            Color(255, 0, 255, 0));
    SolidBrush pedzel(kolor);

    if (e.typ == Ksztalt::Kwadrat)
        g.FillRectangle(&pedzel, e.x, e.y, e.rozmiar, e.rozmiar);
    else if (e.typ == Ksztalt::Kolo)
        g.FillEllipse(&pedzel, e.x, e.y, e.rozmiar, e.rozmiar);
    else {
        Point p[3] = {
            Point(e.x + e.rozmiar / 2, e.y),
            Point(e.x, e.y + e.rozmiar),
            Point(e.x + e.rozmiar, e.y + e.rozmiar)
        };
        g.FillPolygon(&pedzel, p, 3);
    }
    FontFamily ff(L"Arial");
    Font font(&ff, 10);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    WCHAR masaStr[20];
    swprintf(masaStr, 20, L"%d", e.masa);
    g.DrawString(masaStr, -1, &font,
        PointF(e.x + e.rozmiar / 4, e.y + e.rozmiar / 4), &textBrush);

}

void RysujRamie(Graphics& g) {
    Pen pioro(Color(255, 0, 0, 0), 4);
    g.DrawLine(&pioro, ramieX, 0, ramieX, ramieY);
}

bool CzyCosNaWierzchu(const Element& e) {
    for (const auto& inny : elementy) {
        if (&inny == &e || inny.czyPodniesiony) continue;
        int x1 = e.x + e.rozmiar / 2;
        int y1 = e.y;
        int x2 = inny.x + inny.rozmiar / 2;
        int y2 = inny.y + inny.rozmiar;
        if (abs(x1 - x2) < e.rozmiar && abs(y1 - y2) <= 2)
            return true;
    }
    return false;
}
// Zwraca element wybranego typu, leżący na ziemi, masa <= 500 i bez niczego na sobie
Element* ZnajdzElement(Ksztalt ksztalt) {
    for (auto& e : elementy)
        if (
            e.typ == ksztalt &&
            e.y == 400 &&
            e.masa <= 500 &&
            !CzyCosNaWierzchu(e)
            )
            return &e;
    return nullptr;
}
// Uruchamia animację budowania wieży
void StartAnimacjaWiezy(HWND hwnd, Element* dol, Element* srodek, Element* gora) {
    animacjaEtap = 1;
    animEl1 = dol;
    animEl2 = srodek;
    animEl3 = gora;
    ramieX = dol->x + dol->rozmiar / 2;
    ramieY = dol->y - 40;
    SetTimer(hwnd, 1, 15, NULL);
}
// Wywołaj dla klawisza 5 (kwadrat -> trójkąt -> koło)
void BudujAutomatycznie(HWND hwnd) {
    automatyczneBudowanie = true;
    Element* kwadrat = ZnajdzElement(Ksztalt::Kwadrat);
    Element* trojkat = ZnajdzElement(Ksztalt::Trojkat);
    Element* kolo = ZnajdzElement(Ksztalt::Kolo);
    if (!kwadrat || !trojkat || !kolo) {
        MessageBox(hwnd, L"Brakuje wymaganych figur (każdy typ <= 500 kg) na ziemi!", L"Błąd", MB_OK | MB_ICONWARNING);
        automatyczneBudowanie = false;
        return;
    }
    StartAnimacjaWiezy(hwnd, kwadrat, trojkat, kolo);
}
// Wywołaj dla klawisza 6 (koło -> kwadrat -> trójkąt)
void BudujAutomatycznie2(HWND hwnd) {
    automatyczneBudowanie = true;
    Element* kolo = ZnajdzElement(Ksztalt::Kolo);
    Element* kwadrat = ZnajdzElement(Ksztalt::Kwadrat);
    Element* trojkat = ZnajdzElement(Ksztalt::Trojkat);
    if (!kolo || !kwadrat || !trojkat) {
        MessageBox(hwnd, L"Brakuje wymaganych figur (każdy typ <= 500 kg) na ziemi!", L"Błąd", MB_OK | MB_ICONWARNING);
        automatyczneBudowanie = false;
        return;
    }
    StartAnimacjaWiezy(hwnd, kolo, kwadrat, trojkat);
}

int SumaKulWWiezach() {
    int suma = 1;
    for (const auto& para : wieze)
        suma += (int)para.second.size();
    return suma;
}
extern HINSTANCE g_hInstance;

int PodajMase(HWND hwnd, HINSTANCE hInstance) {
    int masa = -1;

    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MASA_DIALOG), hwnd,
        [](HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
            static int* pMasa = nullptr;

            switch (msg) {
            case WM_INITDIALOG:
                pMasa = (int*)lParam;
                return TRUE;

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                case IDOK: {
                    wchar_t buf[16];
                    GetDlgItemText(hDlg, IDC_EDIT_MASA, buf, 16);
                    if (pMasa) *pMasa = _wtoi(buf);
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }
                break;
            }
            return FALSE;
        }, (LPARAM)&masa);

    return masa;
}



int k = 0;
int l = 0;
int m = 0;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Graphics g(hdc);
        g.Clear(Color(255, 255, 255, 255));

        SolidBrush ziemia(Color(255, 200, 200, 200));
        g.FillRectangle(&ziemia, 0, wysokoscPodloza, 2000, 600 - wysokoscPodloza);

        FontFamily ff(L"Arial");
        Font font(&ff, 16);
        SolidBrush czarny(Color(255, 0, 0, 0));
        WCHAR tekst[600];
        wsprintf(tekst, L"Aby dodać element/rozpocząć animacje kliknij:\n -'1'-koło\n -'2'-kwadrat\n -'3'-trójkąt\n -'5'-kwadtar->trójkąt->koło\n -'6'-koło->kwadrat->trójkąt");
        g.DrawString(tekst, -1, &font, PointF(10, 10), &czarny);

        RysujRamie(g);
        for (auto& e : elementy)
            RysujElement(g, e, &e == podniesiony);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_LEFT:  ramieX -= 10; break;
        case VK_RIGHT: ramieX += 10; break;
        case VK_UP:    ramieY -= 10; break;
        case VK_DOWN:
            if (ramieY + 10 < wysokoscPodloza)
                ramieY += 10;
            break;
            // Dodawanie nowych elementów:
        case '1': { // dodaj koło


            int masa = PodajMase(hwnd, GetModuleHandle(NULL));

            if (masa == -1) break; // anulowano

            elementy.push_back({ Ksztalt::Kolo, 100+k, 400, 50, masa });
            Element* nowy = &elementy.back();
            wieze[nowy] = {};
            k += 225;
            break;
        }


        case '2': { // dodaj kwadrat


            int masa = PodajMase(hwnd, GetModuleHandle(NULL));

            if (masa == -1) break; // anulowano

            elementy.push_back({ Ksztalt::Kwadrat, 175+l, 400, 50, masa });
            Element* nowy = &elementy.back();
            wieze[nowy] = {};
            l += 225;
            break;

        }
        case '3': { // dodaj trójkąt


            int masa = PodajMase(hwnd, GetModuleHandle(NULL));

            if (masa == -1) break; // anulowano

            elementy.push_back({ Ksztalt::Trojkat, 250+m, 400, 50, masa });
            Element* nowy = &elementy.back();
            wieze[nowy] = {};
            m += 225;
            break;
        }
        case '5':
            if (!automatyczneBudowanie)
                BudujAutomatycznie(hwnd);
            break;
        case '6':
            if (!automatyczneBudowanie)
                BudujAutomatycznie2(hwnd);
            break;

        case VK_SPACE:
            if (podniesiony == nullptr) {
                for (auto& e : elementy) {
                    if (e.masa > 500) {
                        MessageBox(hwnd, L"Nie można podnieść tego obiektu — jest za ciężki. Spróbuj podnieść inny element", L"Błąd", MB_OK | MB_ICONWARNING);
                        break;
                    }
                    if (e.typ != dozwolonyKsztalt || e.czyPodniesiony || e.masa > dowolnamasa) continue;
                    int srodekX = e.x + e.rozmiar / 2;
                    int srodekY = e.y + e.rozmiar / 2;
                    if (abs(srodekX - ramieX) < 30 && abs(srodekY - ramieY) < 30) {
                        bool cosNaNim = false;
                        for (const auto& para : wieze) {
                            const auto& stos = para.second;
                            auto it = std::find(stos.begin(), stos.end(), &e);
                            if (it != stos.end() && it != std::prev(stos.end())) {
                                cosNaNim = true;
                                break;
                            }
                        }
                        if (cosNaNim || CzyCosNaWierzchu(e)) {
                            MessageBox(hwnd, L"Nie można podnieść tej figury — coś na niej leży.", L"Błąd", MB_OK | MB_ICONWARNING);
                            break;
                        }

                        podniesiony = &e;
                        pomocny = &e;
                        e.czyPodniesiony = true;
                        for (auto& para : wieze) {
                            auto& stos = para.second;
                            stos.erase(std::remove(stos.begin(), stos.end(), &e), stos.end());
                        }
                        wieze.erase(&e);
                        break;
                    }
                }
            }

            else {
                if (k == 0) {
                    podniesiony->x = ramieX - podniesiony->rozmiar / 2;
                }

                Element* baza = nullptr;
                for (auto& e : elementy) {
                    if (&e == podniesiony || e.czyPodniesiony) continue;
                    if (e.y != 400) continue;
                    int srodekX = e.x + e.rozmiar / 2;
                    if (abs(srodekX - ramieX) < 30) {
                        baza = &e;
                        break;
                    }
                }

                if (baza) {
                    {
                        if (baza->typ != podniesiony->typ) {

                            MessageBox(hwnd, L"Nie można układać różnych kształtów na sobie.", L"Błąd", MB_OK | MB_ICONWARNING);

                            LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

                        }
                        else {
                            auto& stos = wieze[baza];
                            if (stos.size() < 2) {
                                podniesiony->x = baza->x;
                                podniesiony->y = baza->y - (int)(stos.size() + 1) * podniesiony->rozmiar;
                                stos.push_back(podniesiony);


                            }
                            else {
                                MessageBox(hwnd, L"Zbyt wysoka wieża. Grozi zawaleniu!", L"Błąd krytyczny (załóż kask)", MB_OK | MB_ICONWARNING);
                                LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


                            }
                        }
                    }
                }

                else {
                    podniesiony->y = 400;
                    wieze[podniesiony] = {};
                }


                podniesiony->czyPodniesiony = false;
                podniesiony = nullptr;
            }
            break;
        }

        if (podniesiony) {
            podniesiony->x = ramieX - podniesiony->rozmiar / 2;
            podniesiony->y = ramieY;
        }

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
        case WM_TIMER: {
            if (animacjaEtap == 1) { // Podnoś środkowy element (drugi)
                int celX = animEl2->x + animEl2->rozmiar / 2;
                int celY = animEl2->y + animEl2->rozmiar / 2 - 50;
                if (ramieX < celX) ramieX += 4;
                if (ramieX > celX) ramieX -= 4;
                if (ramieY < celY) ramieY += 4;
                if (ramieY > celY) ramieY -= 4;
                if (abs(ramieX - celX) < 5 && abs(ramieY - celY) < 5) {
                    animEl2->czyPodniesiony = true;
                    animacjaEtap = 2;
                }
            }
            else if (animacjaEtap == 2) { // Przenoszenie srodkowego elementu (ramie trzyma element)
                int celX = animEl1->x;
                int celY = animEl1->y - animEl2->rozmiar;
                if (animEl2->x < celX) animEl2->x += 4;
                if (animEl2->x > celX) animEl2->x -= 4;
                if (animEl2->y < celY) animEl2->y += 4;
                if (animEl2->y > celY) animEl2->y -= 4;
                // Ramię dokładnie do środka górnej krawędzi podnoszonego elementu:
                ramieX = animEl2->x + animEl2->rozmiar / 2;
                ramieY = animEl2->y;
                if (abs(animEl2->x - celX) < 5 && abs(animEl2->y - celY) < 5) {
                    animEl2->x = celX;
                    animEl2->y = celY;
                    animEl2->czyPodniesiony = false;
                    animacjaEtap = 3;
                }
            }
            else if (animacjaEtap == 3) { // Podnoś trzeci element (górny)
                int celX = animEl3->x + animEl3->rozmiar / 2;
                int celY = animEl3->y + animEl3->rozmiar / 2 - 50;
                if (ramieX < celX) ramieX += 4;
                if (ramieX > celX) ramieX -= 4;
                if (ramieY < celY) ramieY += 4;
                if (ramieY > celY) ramieY -= 4;
                if (abs(ramieX - celX) < 5 && abs(ramieY - celY) < 5) {
                    animEl3->czyPodniesiony = true;
                    animacjaEtap = 4;
                }
            }
            else if (animacjaEtap == 4) { // Przenoszenie gornego elementu (ramie trzyma element)
                int celX = animEl1->x;
                int celY = animEl2->y - animEl3->rozmiar;
                if (animEl3->x < celX) animEl3->x += 4;
                if (animEl3->x > celX) animEl3->x -= 4;
                if (animEl3->y < celY) animEl3->y += 4;
                if (animEl3->y > celY) animEl3->y -= 4;
                ramieX = animEl3->x + animEl3->rozmiar / 2;
                ramieY = animEl3->y;
                if (abs(animEl3->x - celX) < 5 && abs(animEl3->y - celY) < 5) {
                    animEl3->x = celX;
                    animEl3->y = celY;
                    animEl3->czyPodniesiony = false;
                    animacjaEtap = 0;
                    automatyczneBudowanie = false;
                    KillTimer(hwnd, 1);
                }
            }
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
void jeszczerazspacja() {


    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

}
HINSTANCE g_hInstance;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    g_hInstance = hInst;
    InicjalizujGDIPlus();

    int wynik = MessageBox(NULL,
        L"Wybierz typ elementów, które może podnosić dźwig:\nTAK - Kule\nNIE - Kwadraty\nANULUJ - Trójkąty.\n\n WAŻNE!, aby generować nowe elementy wciśnij 1-kula, 2-kwadrat, 3-trójkąt.",
        L"Wybierz tryb", MB_YESNOCANCEL | MB_ICONQUESTION);

    switch (wynik) {
    case IDYES:
        dozwolonyKsztalt = Ksztalt::Kolo;
        break;
    case IDNO:
        dozwolonyKsztalt = Ksztalt::Kwadrat;
        break;
    case IDCANCEL:
        dozwolonyKsztalt = Ksztalt::Trojkat;
        break;
    default:
        // Domyślnie kule
        dozwolonyKsztalt = Ksztalt::Kolo;
    }
    const wchar_t* KL = L"DZWIG";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = KL;
    RegisterClass(&wc);


    HWND hwnd = CreateWindowEx(0, KL, L"Symulator Dźwigu", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ZakonczGDIPlus();
    return 0;
}
