# Testy sprawdzające efektywność funkcji grupowych

Testy zawarte w tym katalogu sprawdzają rozwiązanie pod kątem efektywnej implementacji funkcji grupowych
jak wyspecyfikowano w pliku `assignment.md`.

Dozwolone czasy wykonania (limit `timeout`) zapewnionych testów wynika z formuły z treści $3\left \lceil\log_2(n+1)-1 \right \rceil t+\epsilon$,
w której:

- liczba procesów $n$ to `3,15` albo `16` w zależności od testu
- $\epsilon$ - opóźnienie dodatkowe to `100ms`
- opóźnione są jednie odczyty (wywołania funkcji `chsend`) o `100ms` albo `50ms`.
  Zatem $t$ najdłuższy czas przesłania pojedynczej wiadomości (wysłanie + odczyt)
  to każdorazowo czas wysłania.

Warto zwrócić uwagę, że treść zadania `assignment.md` specyfikuje, że
wiadomości przesyłane z użyciem `chsend` przy wymienianiu małych danych powinny być odpowiednio małe.
Zapewnione testy, sprawdzając ten warunek korzystają z tego,
że implementacja `channel.c` ma opóźnienia zależne od długości wysyłanej wiadomości.

## Na przykładzie

Dla konkretnych grup testów mamy:

| grupa testów     | $n$ | czas zapisu | czas odczytu | $t$   | $\epsilon$ | sumaryczny czas |
| ---------------- | --- | ----------- | ------------ | ----- | ---------- | --------------- |
| *_3_delay_100ms  | 3   | 100ms       | 0ms          | 100ms | 100ms      | 400ms           |
| *_15_delay_100ms | 15  | 100ms       | 0ms          | 100ms | 100ms      | 1000ms          |
| *_16_delay_50ms  | 16  | 50ms        | 0ms          | 50ms  | 100ms      | 700ms           |
