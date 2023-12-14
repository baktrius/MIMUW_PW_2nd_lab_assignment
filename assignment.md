# MIMPI

[MPI](https://pl.wikipedia.org/wiki/Message_Passing_Interface)
to standardowy protokół komunikacyjny służący do wymieniania danych
między procesami programów równoległych, używany przede wszystkim w obliczeniach superkomputerowych.
Celem zadania, jak sugeruje nazwa _MIMPI_ - będąca skrótem od _My Implementation of MPI_ - jest
zaimplementowanie drobnego, nieco zmodyfikowanego fragmentu MPI.
Należy napisać według poniższej specyfikacji zarówno:

- kod programu `mimpirun` (w `mimpirun.c`) uruchamiający obliczenia równoległe,
- implementację `mimpi.c` procedur zadeklarowanych w `mimpi.h`.

## Program `mimpirun`

Program `mimpirun` przyjmuje następujące argumenty linii poleceń:

1) $n$ - liczba kopii do uruchomienia (można założyć, że przekazana zostanie liczba naturalna z zakresu od 1 do $16$ włącznie)
2) $prog$ - ścieżka do pliku wykonywalnego (może się znajdować w PATH).
   W przypadku, gdy odpowiednie wywołanie `exec` się nie powiedzie (np. z powodu niepoprawnej ścieżki)
   należy zakończyć działanie `mimpirun` z niezerowym kodem wyjściowym.
3) $args$ - opcjonalnie i w dowolnej ilości argumenty do przekazania wszystkim uruchamianym programom $prog$

Program `mimpirun` po kolei (następna czynność jest rozpoczynana po całkowitym zakończeniu poprzedniej):

1) Przygotowuje środowisko (w zakresie implementującego jest zdecydowanie, co to znaczy).
2) Uruchamia $n$ kopii programu $prog$, każdą w osobnym procesie.
3) Czeka na zakończenie wszystkich utworzonych procesów.
4) Kończy działanie.

## Założenia o programach $prog$

- Programy $prog$ mogą **jednokrotnie** w czasie swojego działania przejść do _bloku MPI_.
Aby tego dokonać wywołują na początku funkcję biblioteczną `MIMPI_Init`,
a na jego końcu wykonują funkcję biblioteczną `MIMPI_Finalize`. Jako wspomniany _blok MPI_
rozumie się cały fragment kodu pomiędzy w/w wywołaniami.
- Będąc w _bloku MPI_, programy mogą wykonywać różne procedury z biblioteki `mimpi`
celem komunikacji z innymi procesami $prog$.
- Mogą wykonywać dowolne operacje (zapis, odczyt, otwarcie, zamknięcie, itp.) na plikach,
  których numery deskryptorów są z zakresów $[0,19]$ i $[1024, \infty)$
  (w szczególności na `STDIN`, `STDOUT` i `STDERR`).
- Nie modyfikują wartości zmiennych środowiskowych zaczynających się od prefiksu `MIMPI`.
- Oczekują prawidłowo ustawionych argumentów,
tzn. zerowy argument zgodnie z uniksową konwencją powinien być nazwą programu $prog$,
natomiast następne argumenty powinny odpowiadać argumentom $args$.
_Wskazówka:_ aby przekazać dodatkowe dane z `mimpirun` do $prog$ można posłużyć się funkcjami z rodziny `*env`: `getenv`, `setenv`, `putenv`.

## Biblioteka `mimpi`

Należy zaimplementować następujące procedury o sygnaturach z pliku nagłówkowego `mimpi.h`:

### Procedury pomocnicze

- `void MIMPI_Init(bool enable_deadlock_detection)`

  Otwiera _blok MPI_, inicjalizując potrzebne dla działania biblioteki `mimpi` zasoby.
  Flaga `enable_deadlock_detection` włącza wykrywanie zakleszczeń do końca _bloku MPI_ (opis poniżej w **Usprawnienie4**).

- `void MIMPI_Finalize()`

  Kończy _blok MPI_.
  Wszystkie zasoby związane z działaniem biblioteki `mimpi`:
  - otwarte pliki
  - otwarte kanały komunikacyjne
  - zaalokowana pamięć
  - prymitywy synchronizacyjne
  - itp.
  
  powinny być zwolnione przed zakończeniem tej procedury.

- `int MIMPI_World_size()`

  Zwraca liczbę procesów $prog$ uruchomionych z użyciem programu `mimpirun` (powinno być równe parametrowi $n$ przekazanemu do wywołania `mimpirun`).

- `void MIMPI_World_rank()`

  Zwraca unikatowy w ramach grupy procesów uruchomionych przez `mimpirun` identyfikator.
  Identyfikatory powinny być kolejnymi liczbami naturalnymi od $0$ do $n-1$.

### Procedury do komunikacji punkt-punkt

- `MIMPI_Retcode MIMPI_Send(void const *data, int count, int destination, int tag)`

  Wysyła dane spod adresu `data`, interpretując je jako tablicę bajtów o wielkości `count`,
  do procesu o randze `destination`, opatrując wiadomość znacznikiem `tag`.

  Wykonanie `MIMPI_Send` na rzecz procesu, który już opuścił _blok MPI_ powinno
  zakończyć się natychmiastowym niepowodzeniem zwracając kod błędu `MIMPI_ERROR_REMOTE_FINISHED`.
  Nie należy przejmować się sytuacją, w której proces na rzecz którego wykonane zostało
  `MIMPI_Send` zakończy działanie później (po pomyślnym zakończeniu funkcji `MIMPI_send` w procesie wysyłającym).

- `MIMPI_Retcode MIMPI_Recv(void *data, int count, int source, int tag)`

  Czeka na wiadomość o wielkości (dokładnie) `count` i znaczniku `tag` od procesu
  o randze `rank` i zapisuje jej zawartość pod adresem `data`
  (w gestii wołającego jest zapewnienie odpowiedniej ilości zaalokowanej pamięci).
  Wywołanie jest blokujące, tzn. kończy się dopiero po otrzymaniu całej wiadomości.

  Wykonanie `MIMPI_Recv` na rzecz procesu, który
  nie wysłał pasującej wiadomości i opuścił już _blok MPI_ powinno
  zakończyć się niepowodzeniem zwracając kod błędu `MIMPI_ERROR_REMOTE_FINISHED`.
  Podobne zachowanie jest oczekiwane nawet w sytuacji gdy drugi proces opuści _blok MPI_
  już podczas czekania na `MIMPI_Recv`.

  - Wersja podstawowa: można zakładać, że każdy z procesów wysyła komunikaty w dokładnie takiej kolejności,
    w jakiej odbiorca chce je odbierać. **Nie można** natomiast zakładać, że wiele procesów **nie wyśle** równocześnie wiadomości do jednego odbiorcy. Można zakładać, że dane powiązane z jedną wiadomością są nie większe niż 512 bajtów.
  - **Usprawnienie1**: Przesyłane wiadomości mogą być dowolnie (rozsądnie) duże, w szczególności większe niż bufor łącza (`pipe`).
  - **Usprawnienie2**: Nie można zakładać nic o kolejności wysłanych pakietów.
    Odbiorca powinien być w stanie buforować przychodzące pakiety, a w momencie wywołania `MIMPI_Recv` zwracać pierwszą
    (ze względu na czas przyjścia) wiadomość pasującą parametrami `count`, `source` i `tag`
    (nie trzeba implementować bardzo wymyślnego mechanizmu wybierania następnego pasującego pakietu;
    złożoność liniowa względem liczby wszystkich jeszcze nieprzetworzonych przez docelowy proces pakietów
    jest w zupełności wystarczająca).
  - **Usprawnienie3**: Odbiorca powinien przetwarzać przysyłane wiadomości
    współbieżnie z wykonywaniem innych czynności, tak by nie doszło do przepełnienia kanałów do wysyłania wiadomości.
    Innymi słowy wysłanie dużej ilości wiadomości nie jest blokujące nawet jeśli docelowy odbiorca ich nie przetwarza (ponieważ trafiają do wciąż rosnącego bufora).
  - **Usprawnienie4**: Zakleszczenie to sytuacja,
    w której część systemu znalazła się w stanie, który nie ma już żadnej możliwości się zmienić
    (nie istnieje taka sekwencja możliwych przyszłych zachowań procesów, które by to zakleszczenie rozwiązywały).
    Zakleszczenie pary procesów to
    sytuacja, w których zakleszczenie jest spowodowane stanem dwóch procesów
    (rozważając czy można je przerwać dopuszczamy dowolne akcje procesów spoza pary - nawet takie, które nie są dozwolone w ich bieżącym stanie).

    Przykłady pewnych sytuacji będącymi zakleszczeniami par procesów w naszym systemie to:
    1) para procesów wykonała wzajemnie na siebie `MIMPI_Recv` nie wysyłając uprzednio z użyciem `MIMPI_Send` wiadomości, która może zakończyć czekanie któregokolwiek z nich
    2) jeden z procesów czeka na wiadomość od procesu,
    który czeka już na synchronizację związaną z wywołaniem procedury do komunikacji grupowej

    W ramach tego usprawnienia należy zaimplementować przynajmniej wykrywanie zakleszczeń par typu 1).
    Wykrywanie zakleszczeń innych typów nie będzie sprawdzane (można je zaimplementować).
    Nie należy natomiast zgłaszać zakleszczeń w sytuacjach, które zakleszczeniami nie są.

    W przypadku wykrycia zakleszczenia aktywne wywołanie funkcji bibliotecznej `MIMPI_Recv` w **obu procesach** wykrytego zakleszczenia pary powinno natychmiast się zakończyć zwracając kod błędu `MIMPI_ERROR_DEADLOCK_DETECTED`.

    W przypadku wystąpienia wielu zakleszczonych par jednocześnie należy przerwać wywołanie funkcji bibliotecznej `MIMPI_Recv`
    w każdym procesie każdej zakleszczonej pary.

    Detekcja zakleszczeń może do działania wymagać wysyłania wielu pomocniczych komunikatów, co może istotnie spowalniać działanie systemu.
    Dlatego funkcjonalność tę można włączać i wyłączać na czas działania całego _bloku MPI_ ustawiając odpowiednią wartość flagi `enable_deadlock detection` w wywołaniu `MIMPI_Init` rozpoczynającym ten blok.

    Działanie w przypadku, gdy detekcję zakleszczeń włączono jedynie w niektórych procesach aktualnego wykonania `mimpirun` jest niezdefiniowane.

    **Uwaga**: _usprawnienie4_ (wykrywanie zakleszczeń) wymaga usprawnienia2 (filtrowanie wiadomości). Częściowe wykrywanie zakleszczeń - bez implementacji _usprawnienia2_ - będzie oceniane na 0 punktów.

### Procedury do komunikacji grupowej

#### Wymagania ogólne

Każda procedura $p$ do komunikacji grupowej stanowi **punkt synchronizacyjny** wszystkich procesów,
tzn. instrukcje następujące po $i$-tym wywołaniu $p$ w dowolnym procesie wykonują się **po** każdej instrukcji poprzedzającej $i$-te wywołanie $p$ w dowolnym innym procesie.

W przypadku, jeśli synchronizacja wszystkich procesów nie może się zakończyć,
bo któryś z procesów opuścił już _blok MPI_, wywołanie `MIMPI_Barrier` w przynajmniej jednym procesie
powinno zakończyć się kodem błędu `MIMPI_ERROR_REMOTE_FINISHED`.
Jeśli proces, w którym się tak stanie w reakcji na błąd sam zakończy działanie,
wywołanie `MIMPI_Barrier` powinno zakończyć się w przynajmniej jednym następnym procesie.
Powtarzając powyższe zachowanie, powinniśmy dojść do sytuacji, w której każdy proces
opuścił barierę z błędem.

#### Efektywność

Każda procedura $p$ do komunikacji grupowej powinna być zrealizowana efektywnie.
Dokładniej, przy założeniu, że detekcja zakleszczeń jest nieaktywna, wymagamy aby od czasu wywołania $p$ przez ostatni proces
do czasu zakończenia $p$ w ostatnim procesie minęło co najwyżej $3\left \lceil\log_2(n+1)-1 \right \rceil t+\epsilon$ czasu gdzie:

- $n$ to liczba procesów
- $t$ to najdłuższy czas przesłania pojedynczej wiadomości - suma czasów wykonania pary odpowiadających sobie wywołań `chsend` i `chrecv`. Dodatkowe informacje można wyczytać z przykładowej implementacji `channel.c` i z zapewnionych testów z katalogu `tests/effectiveness`
- $\epsilon$ to nieduża stała (rzędu co najwyżej milisekund), która nie zależy od $t$.

Dodatkowo aby implementacja została uznana za efektywną przesyłane dane nie powinny być
opatrzone za dużą liczbą metadanych.
W szczególności oczekujemy, że funkcje grupowe wywołane dla danych wielkości
mniejszej niż 256 bajtów będą wywoływać `chsend` i `chrecv` na rzecz pakietów
wielkości mniejszej lub równej niż 512 bajtów.

Testy z katalogu `tests/effectiveness` dołączone w paczce sprawdzają powyżej zdefiniowane pojęcie efektywności.
Przejście ich pozytywnie jest warunkiem koniecznym (choć niekoniecznie wystarczającym)
do zdobycia punktów za efektywną implementację funkcji grupowych.

#### Dostępne procedury

- `MIMPI_Retcode MIMPI_Barrier()`

  Przeprowadza synchronizację wszystkich procesów.

- `MIMPI_Retcode MIMPI_Bcast(void *data, int count, int root)`

  Wysyła dane zapewnione przez proces o randze `root` do wszystkich pozostałych procesów.

- `MIMPI_Retcode MIMPI_Reduce(const void *send_data, void *recv_data, int count, MPI_Op op, int root)`

  Zbiera dane zapewnione przez wszystkie procesy w `send_data`
  (traktując je jak tablicę liczb typu `uint8_t` wielkości `count`)
  i przeprowadza na elementach o tych samych indeksach
  z tablic `send_data` wszystkich procesów (również `root`) redukcję typu `op`.
  Wynik redukcji, czyli tablica typu `uint8_t` wielkości `count`,
  jest zapisywany pod adres `recv_data` **wyłącznie** w procesie o randze `root` (**niedozwolony** jest zapis pod adres `recv_data` w pozostałych procesach).

  Dostępne są następujące typy redukcji (wartości `enum`a `MIMPI_Op`):
  - `MIMPI_MAX`: maksimum
  - `MIMPI_MIN`: minimum
  - `MIMPI_SUM`: suma
  - `MIMPI_PROD`: produkt

  Należy zwrócić uwagę na to, że wszystkie powyższe operacje na dostępnych typach danych
  są przemienne i łączne i odpowiednio zoptymalizować `MIMPI_Reduce`.

### Semantyka `MIMPI_Retcode`

Patrz dokumentacja w kodzie `mimpi.h`:

- dokumentacja `MIMPI_Retcode`,
- dokumentacja poszczególnych procedur zwracających `MIMPI_Retcode`.

### Semantyka znaczników

Przyjmujemy konwencję:

- `tag > 0` jest przeznaczony dla użytkowników biblioteki na własne potrzeby,
- `tag = 0` oznacza `ANY_TAG`. Jego zastosowanie do `MIMPI_Recv` powoduje dopasowanie do dowolnego znacznika.
Nie należy go używać w `MIMPI_Send` (skutek użycia jest niezdefiniowany).
- `tag < 0` jest zarezerwowany na potrzeby implementujących bibliotekę i może być zastosowany do wewnętrznej komunikacji.

  W szczególności oznacza to, że programy użytkownika (np. nasze programy testowe) nigdy nie wywołają bezpośrednio procedury `MIMPI_Send` czy `MIMPI_Recv` ze znacznikiem `< 0`.

## Komunikacja międzyprocesowa

Standard MPI jest zaprojektowany z myślą o obliczeniach uruchamianych na superkomputerach.
W związku z tym komunikacja między poszczególnymi procesami zazwyczaj odbywa się przez sieć
i jest wolniejsza niż wymiana danych w obrębie jednego komputera.

Aby lepiej zasymulować środowisko działania prawdziwej biblioteki
i tym samym zmierzyć się z jej problemami implementacyjnymi,
komunikację między procesami należy przeprowadzić **wyłącznie**
z użyciem, dostarczonych w bibliotece `channel.h`, kanałów.
Biblioteka `channel.h` zapewnia następujące funkcje do obsługi kanałów:

- `void channels_init()` - inicjalizacja biblioteki kanałów
- `void channels_finalize()` - finalizacja biblioteki kanałów
- `int channel(int pipefd[2])` - utworzenie kanału
- `int chsend(int __fd, const void *__buf, size_t __n)` - wysłanie wiadomości
- `int chrecv(int __fd, void *__buf, size_t __nbytes)` - odebranie wiadomości

`channel`, `chsend`, `chrecv` działają podobnie do `pipe`, `write` i `read` odpowiednio.
Zamysł jest taki, że jedyna istotna (z perspektywy rozwiązania zadania)
różnica w zachowaniu funkcji zapewnionych przez `channel.h` jest
taka, że mogą one mieć znacząco dłuższy czas wykonania od swoich oryginałów.
W szczególności zapewnione funkcje:

- posiadają taką samą sygnaturę jak oryginalne funkcje
- podobnie tworzą wpisy w tablicy otwartych plików
- gwarantują atomowość odczytów i zapisów do 512 bajtów włącznie
- gwarantują posiadanie bufora o wielkości przynajmniej 4 KB
- ... _(w razie niejasności należy zadać pytanie)_

**UWAGA:**
Należy koniecznie wywołać następujące funkcje pomocnicze: `channels_init` z `MIMPI_Init`,
zaś `channels_finalize` z `MIMPI_Finalize`.

**Wszystkie** odczyty i zapisy na rzecz deskryptorów plików zwróconych przez funkcję `channel`
należy wykonywać z użyciem `chsend` i `chrecv`.
Dodatkowo, nie należy wywoływać żadnych funkcji systemowych modyfikujących własności plików jak `fcntl`
na rzecz deskryptorów plików zwróconych przez funkcję `channel`.

Niezastosowanie się do powyższych zaleceń może skutkować **całkowitą utratą punktów.**

Należy pamiętać, że z powyższych gwarancji dotyczących funkcji `chsend` oraz `chrecv`
nie wynika, że nie przetworzą one mniejszej liczby bajtów niż żądana.
Może się tak stać jeśli wielkość ta przekracza gwarantowany rozmiar bufora kanału,
lub gdy ilość danych w buforze wejściowym jest niewystarczająca.
Implementacje muszą poprawnie obsługiwać taką sytuację.

## Uwagi

### Ogólne

- Program `mimpirun` ani żadna z funkcji z biblioteki `mimpi` **nie mogą** tworzyć nazwanych plików w systemie plików.
- Program `mimpirun` i funkcje z biblioteki `mimpi` mogą korzystać z deskryptorów
  o numerach należących do przedziału $[ 20, 1023 ]$ w dowolny sposób.
  Dodatkowo można zakładać, że deskryptory z powyższego zakresu nie są zajęte w momencie
  uruchomienia programu `mimpirun`.
- Program `mimpirun` ani żadna z funkcji z biblioteki `mimpi` **nie mogą** modyfikować istniejących
  wpisów z tablicy otwartych plików z pozycji spoza $[ 20, 1023 ]$.
- Program `mimpirun` ani żadna z funkcji z biblioteki `mimpi` **nie mogą** wykonywać żadnych
  operacji na plikach, których same nie otworzyły (w szczególności na `STDIN`, `STDOUT` i `STDERR`).
- W żadnym miejscu nie można stosować aktywnego ani półaktywnego oczekiwania.
  - Tym samym nie należy korzystać z żadnej funkcji usypiającej wykonanie programu
  na określony czas (`sleep`, `usleep`, `nanosleep`) ani też wariantów funkcji z timeout'em (jak np. `select`).
  - Należy czekać **wyłącznie** na wydarzenia niezależne od czasu np. pojawienie się wiadomości.
- Rozwiązania będą testowane pod kątem wycieków pamięci i/lub innych zasobów (niezamkniętych plików itd.).
  Należy uważnie prześledzić i przetestować ścieżki mogące prowadzić do wycieków.
- Można założyć, że odpowiadające i-te wywołania funkcji do komunikacji grupowej
  w różnych procesach są tych samych typów (są to te same funkcje) i mają takie same wartości parametrów `count`, `root` i `op`
  (jeśli bieżący typ funkcji posiada dany parametr).
- W przypadku błędu w funkcji systemowej należy zakończyć wywołujący ją program z niezerowym kodem wyjściowym np.
  poprzez użycie dostarczonego makra `ASSERT_SYS_OK`.
- W przypadku, gdy programy $prog$ korzystają z biblioteki w sposób niezgodny z wymienionymi w tej treści gwarancjami,
  można postąpić dowolnie (nie będziemy takich sytuacji sprawdzać).

### biblioteka `MIMPI`

- Implementowane funkcje nie muszą być _threadsafe_, tzn. można założyć, że nie są wołane z wielu wątków równocześnie.
- Implementacje funkcji powinny być rozsądnie efektywne,
  tzn. niewystawione na ekstremalne obciążenie (np. obsługę setek tysięcy wiadomości)
  nie powinny dodawać poza oczekiwany czas działania (np. wynikający z oczekiwania na wiadomość)
  narzutu rzędu dziesiątek milisekund (i większego).
- Wykonanie procedury innej niż `MIMPI_Init` poza blokiem MPI ma niezdefiniowane działanie.
- Wielokrotne wywołanie procedury `MIMPI_Init` ma niezdefiniowane działanie.
- Dajemy gwarancję, że `channels_init` ustawi obsługę sygnału `SIGPIPE` jako ignorowany. Ułatwi to poradzenie sobie z wymaganiem,
  by `MIMPI_Send` zwrócił `MIMPI_ERROR_REMOTE_FINISHED` w stosownej sytuacji.

## Dopuszczone języki i biblioteki

Wymagamy użycia języka C w wersji `gnu11` (samo `c11` nie daje bowiem dostępu do wielu przydatnych funkcji z biblioteki standardowej).
Nie pozostawiamy wyboru, bowiem zadanie ma m.in. pogłębić umiejętności posługiwania się językiem C.

Można korzystać ze standardowej biblioteki języka C (`libc`), biblioteki `pthread`, a także funkcjonalności dostarczanych przez system (zadeklarowanych w `unistd.h` itp.).

**Niedozwolone**  jest korzystanie z innych bibliotek zewnętrznych.

Mozna zapożyczać dowolny kod z laboratoriów. Wszelkie inne ewentualne zapożyczenia kodu należy odpowiednio komentować z podaniem źródła.

## Opis paczki

Paczka zawiera następujące pliki niebędące częścią rozwiązania:

- `examples/*`: proste przykładowe programy korzystające z biblioteki `mimpi`
- `tests/*`: testy sprawdzające różne konfiguracje uruchomień przykładowych programów z użyciem `mimpirun`
- `assignment.md`: ten opis
- `channel.h`: plik nagłówkowy deklarujący funkcje do obsługi komunikacji międzyprocesowej
- `channel.c`: przykładową implementację `channel.h`
- `mimpi.h`: plik nagłówkowy deklarujący funkcje biblioteki `MIMPI`
- `Makefile`: przykładowy plik automatyzujący kompilację `mimpirun`, programów przykładowych i uruchamianie testów
- `self`: pomocniczy skrypt do uruchamiania testów dostarczonych w zadaniu
- `test`: skrypt lokalnie wykonujący wszystkie testy z katalogu `tests/`
- `test_on_public_repo`: skrypt wykonujący testy według poniżej przedstawionego [schematu](#schemat-oceniania)
- `files_allowed_for_change`: skrypt listujący pliki, które wolno modyfikować
- `template_hash`: plik specyfikujący wersję szablonu, na podstawie którego zostało przygotowane rozwiązanie

Szablony do uzupełnienia:

- `mimpi.c`: plik zawierający szkielety implementacji funkcji biblioteki `MIMPI`
- `mimpirun.c`: plik zawierający szkielet implementacji programu `mimpirun`
- `mimpi_common.h`: plik nagłówkowy przeznaczony do deklaracji wspólnych funkcjonalności biblioteki `MIMPI` i programu `mimpirun`
- `mimpi_common.c`: plik przeznaczony do implementacji wspólnych funkcjonalności biblioteki `MIMPI` i programu `mimpirun`

### Pomocne komendy

- zbudowanie `mimpirun` i wszystkich przykładów z katalogu `examples/`: `make`
- uruchomienie lokalnych testów: `./test`
- uruchomienie testów według oficjalnego schematu: `./test_on_public_repo`
  
  Powyższa komenda pozwala upewnić się, że rozwiązanie spełnia wymogi techniczne wyszczególnione w [schemacie oceniania](#schemat-oceniania).
- wypisanie wszystkich plików otwartych przez procesy uruchomione przez `mimpirun`: np. `./mimpirun 2 ls -l /proc/self/fd`
- śledzenie błędów pamięci, wycieków pamięci i zasobów:
  
  Przydatne może być narzędzie `valgrind`, w szczególności flagi:
  - `--track-origins=yes`
  - `--track-fds=yes`
  
  Mający węższy zakres działania, ale również pomocny do debugowania może być _ASAN_ (Address Sanitizer).
    Uruchamia się go podaniem flagi `-fsanitize=address` do `gcc`.

## Oczekiwane rozwiązanie

### Wariant standardowy (zalecany, prosty i bezpieczny)

Aby ukończyć zadanie należy:

1) Uzupełnić szablon rozwiązania według specyfikacji, zmieniając jedynie pliki wymienione w `files_allowed_for_change` (w szczególności należy uzupełnić przynajmniej pliki `mimpi.c` i `mimpirun.c`).
2) Upewnić się, że rozwiązanie pasuje do wymaganego [schematu oceniania](#schemat-oceniania) wywołując `./test_on_public_repo`
3) Wyeksportować rozwiązanie z użyciem `make assignment.zip && mv assignment.zip ab123456.zip`
   (zamieniając `ab123456` na Twój login ze students)
   i wgrać na czas wynikowe archiwum do moodle.

### Wariant niestandardowy

Jeśli komuś zapewniony szablon się nie podoba i jest bardzo zmotywowany można go zmienić.
Najpierw należy skomunikować się z autorami zadania celem wstępnej weryfikacji potrzeby.
Po pozytywnej weryfikacji trzeba kolejno:

1) Stworzyć fork publicznego repozytorium z rzeczonym szablonem.
2) Zaktualizować stworzonego fork'a dodanymi przez siebie poprawkami **upewniając się, że nie zawierają one fragmentów rozwiązania**
3) Otworzyć _pull request_ do głównego repozytorium, opisując zmiany.
4) W procesie dyskusji poprawki lub nowy szablon mogą zostać zaakceptowane albo odrzucone.
5) Dalej pracować korzystając z nowo-stworzonego szablonu (przełączając się na odpowiedni branch)

Istotne jest aby zaktualizowany szablon pasował do poniższego schematu oceniania,
w szczególności posiadał odpowiednio zaktualizowany plik `template_hash`.

#### O testach studenckich

Polecenie `make` automatycznie próbuje zbudować
wszystkie programy z `examples/` do `examples_build`.
Dalej wywołanie `./test` uruchamia wszystkie rozpoznawane testy z katalogu `tests/`.
Przyjęta jest następująca konwencja:

- pliki `*.self` to specjalny rodzaj testów uruchamiany z użyciem dostarczonego skryptu pomocniczego `self`.
  W pierwszej linijce specyfikują one polecenie do uruchomienia.
  Natomiast od 3-ciej linijki do końca pliku znajduje się oczekiwana wartość `STDOUT`, jaką powinno wygenerować wykonanie powyższego polecenia.
- pliki `*.sh` to dowolne skrypty shell'owe. Mogą one wykonywać dowolną logikę.
  Powinny jedynie zakończyć się kodem $0$ w przypadku sukcesu (przejścia testu) i niezerowym kodem w przypadku wykrycia błedu.

W przypadku przygotowania przez siebie własnych testów pasujących do powyższego schematu zachęcamy do podzielenia się nimi z innymi.
W tym celu należy tylko w jakiś sposób przekazać je autorom zadania (najlepiej _pull request'em_).
Aby zachęcić Państwa do tego wysiłku, postaramy się uruchomić tak opublikowane testy na rozwiązaniu wzorcowym
i dać informację zwrotną gdyby były niepoprawne.

## Schemat oceniania

Wgrane przez Państwa rozwiązania będą budowane i testowane według następującego schematu:

1) Paczka `zip` z rozwiązaniem o odpowiedniej nazwie `ab123456.zip` (podstaw swój login ze students)
   zostanie pobrana z moodle i rozpakowana.
2) Zostanie stworzony czysty klon $K$ publicznego repozytorium zawierającego udostępniony Państwu szablon.
3) W $K$ zostanie wybrana gałąź na podstawie wartości `template_hash` z Państwa rozwiązania.
4) Do $K$ zostaną skopiowane te pliki z Państwa rozwiązania, które są wymienione w `files_allowed_for_change` z **wersji** z $K$
5) Do $K$ zostanie skopiowany przygotowany przez nas zestaw testów (odpowiednie pliki w `examples/` i `tests/`).
6) W $K$ zostanie wywołane `make` celem zbudowania Państwa rozwiązania i wszystkich przykładów.
7) W $K$ zostanie wywołane `./test`
8) Rozwiązanie zostanie ocenione na podstawie zestawu testów, które przeszło Państwa rozwiązanie.

Wykonanie bez błędów powyższego procesu budowania
i przejście następujących automatycznych testów:

- hello
- send_recv

jest warunkiem koniecznym do uzyskania niezerowej liczby punktów.
Aby sprawdzić zgodność swojego rozwiązania z powyższym schematem oceniania należy użyć skryptu `./test_on_public_repo`.
Zachęcamy do skorzystania.

**Uwaga:** W przypadku rozbieżności w wynikach powyższego procesu brane pod uwagę będzie działanie na serwerze `students`.

## Oceniane komponenty rozwiązania

**Uwaga**: poniższa punktacja jest **wyłącznie** orientacyjna i może ulec dowolnym zmianom.
Jej celem jest łatwiejsze oszacowanie trudności/złożoności danej funkcjonalności.

### Podstawa: (3p)

- implementacja programu `mimpirun` + procedur `MIMPI_Init` i `MIMPI_Finalize` + przechodzący poprawnie przykład/test `hello` (1p)
- implementacja funkcji punkt-punkt (przy założeniu wiadomości o wielkości do 500B) (1p)
- implementacja funkcji grupowych (wielkość wiadomości jw.) (1p)

### Usprawnienia (7p)

- efektywne (logarytmiczne) funkcje grupowe (2p)
  - `MIMPI_Barrier` (1p)
  - `MIMPI_Bcast` oraz `MIMPI_Reduce` (1p)
- (`MIMPI_Recv`::Usprawnienie1) dowolnie duże wiadomości (dowolnie ponad 500B) (1p)
- (`MIMPI_Recv`::Usprawnienie2) obsługa dowolnej kolejności przesyłania wiadomości (filtrowanie po tagach, rozmiarze), w tym obsługa tagu `MIMPI_ANY_TAG` (1p)
- (`MIMPI_Recv`::Usprawnienie3) niezapychalne kanały (1p)
  - brak górnego ograniczenia na wielkość danych do momentu zablokowania procesu wysyłającego
  - brak oczekiwania przy wywołaniu `MIMPI_Send`
- (`MIMPI_Recv`::Usprawnienie4) detekcja zakleszczeń par procesów (2p)

Usprawnienia zapisaliśmy w kolejności, w jakiej sugerujemy ich priorytetyzację.
