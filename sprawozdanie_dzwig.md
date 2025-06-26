# Sprawozdanie z projektu: Symulator Dźwigu (C++/GDI+)

## 1. Cel i założenia projektu

Celem projektu było stworzenie graficznego symulatora dźwigu w środowisku Windows, który umożliwia tworzenie, podnoszenie i układanie na sobie elementów o różnych kształtach (koła, kwadraty, trójkąty) i masie, przy zachowaniu ograniczeń fizycznych i logicznych, np. maksymalnej masy elementów czy wysokości wieży. Program korzysta z biblioteki **GDI+** do rysowania elementów i realizacji animacji.

---

## 2. Zastosowane technologie

- **Język programowania:** C++
- **API graficzne:** GDI+ (Graphics Device Interface Plus)
- **Środowisko:** Windows, WinAPI
- **Elementy GUI:** Dialogi Windows, okna komunikatów, obsługa klawiatury

---

## 3. Opis działania programu

### 3.1. Uruchomienie i wybór trybu

Po uruchomieniu programu użytkownik wybiera, **jaki typ elementów może podnosić dźwig**:

- TAK – kule,
- NIE – kwadraty,
- ANULUJ – trójkąty.

Wybór ten wpływa na dalszą interakcję: dźwig będzie mógł podnosić tylko wybrane kształty.

---

### 3.2. Sterowanie i interakcja

- **Strzałki:** przesuwają ramię dźwigu (lewo/prawo/góra/dół)
- **1:** dodaje nową kulę na ziemi (wybór masy w oknie dialogowym)
- **2:** dodaje nowy kwadrat (analogicznie)
- **3:** dodaje nowy trójkąt (analogicznie)
- **Spacja:** chwyta element lub odkłada go w wybranym miejscu (jeśli spełnia warunki)
- **5:** automatycznie buduje wieżę z kwadratu, trójkąta i koła (w tej kolejności)
- **6:** automatycznie buduje wieżę z koła, kwadratu i trójkąta

Elementy nie mogą być mieszane w jednej wieży. Maksymalna liczba elementów w jednej wieży to 3 (ze względów bezpieczeństwa).

---

### 3.3. Ograniczenia i walidacje

- Każdy element ma **masę zadaną przez użytkownika** (okno dialogowe). Jeśli masa > 500 kg, nie można podnieść elementu.
- **Nie można podnosić elementu, jeśli leży na nim inny element** (logika blokady dla środkowych i dolnych poziomów wieży).
- **Nie można układać różnych kształtów na sobie** – program blokuje próbę ułożenia np. koła na kwadracie.
- **Nie można budować wieży wyższej niż 3 poziomy** – w takim przypadku pojawia się komunikat o zagrożeniu zawaleniem.
- Każda nowa figura pojawia się na ziemi w dedykowanym miejscu, z przesunięciem względem poprzednich.

---

## 4. Omówienie kluczowych fragmentów kodu

### 4.1. Struktura Element

```cpp
struct Element {
    Ksztalt typ;
    int x, y;
    int rozmiar;
    int masa;
    bool czyPodniesiony = false;
};
```

Reprezentuje pojedynczy obiekt w symulatorze (kształt, pozycja, rozmiar, masa, stan podniesienia).

---

### 4.2. Obsługa renderowania

Rysowanie elementów realizowane jest w funkcji **RysujElement**:

- Kolor zależny od typu (czerwony – kwadrat, niebieski – koło, zielony – trójkąt, pomarańczowy – zaznaczenie aktywnego)
- Masa elementu jest wyświetlana na jego powierzchni
- Ramię dźwigu rysowane osobno (czarna linia)

---

### 4.3. Obsługa klawiatury (dodawanie, podnoszenie, odkładanie)

- **Dodawanie elementów**: po wciśnięciu `1`, `2` lub `3` program wywołuje dialog do podania masy, a następnie tworzy nowy obiekt odpowiedniego typu.
- **Podnoszenie (spacja)**: można podnieść tylko element bez innych na sobie, w zasięgu ramienia, zgodny z dozwolonym typem i masą ≤ 500 kg.
- **Odkładanie (spacja)**: element można odłożyć na inny tego samego typu (jeśli wieża < 3 poziomy), lub na ziemię (tworzy nową wieżę).

---

### 4.4. Automatyczne budowanie wieży

- Po wciśnięciu `5` lub `6` program wyszukuje odpowiednie elementy na ziemi i uruchamia animację budowania wieży.
- Animacja polega na automatycznym podnoszeniu i układaniu wybranych elementów, z płynnym przesuwaniem ramienia.

---

### 4.5. Logika wież

- Każda figura leżąca na ziemi (y = 400) może być podstawą wieży.
- Elementy układane są jako stosy w mapie `wieze`, osobno dla każdej wieży.
- Po odłożeniu elementu na inną wieżę lub ziemię – wieża jest aktualizowana.

---

## 5. Instrukcja obsługi

1. **Po uruchomieniu wybierz typ elementów, które może podnosić dźwig** (TAK/NIE/ANULUJ).
2. **Dodawaj figury** wciskając 1, 2 lub 3 i podając masę (można podać dowolną liczbę, powyżej 500 kg będą tylko do ozdoby – nie da się ich podnieść).
3. **Przesuwaj ramię** strzałkami.
4. **Podnoś element** spacją – jeśli spełnia warunki.
5. **Odkładaj na inne elementy tego samego typu** (max. 3 w wieży) lub na ziemię.
6. **Automatycznie buduj wieże** wciskając 5 lub 6 (jeśli masz po jednym elemencie każdego typu na ziemi).

---

## 6. Wnioski i podsumowanie

Program realizuje założone funkcjonalności i pokazuje, jak można w prosty sposób zaimplementować interaktywny symulator z użyciem GDI+ oraz WinAPI. Kod umożliwia łatwą rozbudowę o dodatkowe funkcje (np. więcej poziomów, inne ograniczenia czy obsługę myszy). Projekt pozwolił na przećwiczenie zagadnień z zakresu obsługi zdarzeń, grafiki wektorowej i programowania obiektowego w C++.




