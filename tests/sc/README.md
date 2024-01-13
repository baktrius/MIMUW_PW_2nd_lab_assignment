Testy extended_* poszerzają istniejące test próbując wyłapać dodatkowe przypadki brzegowe.
Test extended_pipe_closed uruchamiany dla więcej niż 6 programów wykracza poza wymaganą specyfikację zadania.
Testy send_recv_1.self oraz reasonable_sized_data.self sprawdzają w bezpośredni sposób przesyłanie dużych wiadomości.
Testy big_broadcast.self oraz reasonable_sized_broadcast.self sprawdzają przesyłanie dużych wiadomości przez MIMPI_Broadcast.
Testy big_reduce.self oraz reasonable_sized_reduce.self sprawdzają przesyłanie dużych wiadomości przez MIMPI_Reduce.
Test order_of_msg.self sprawdza czy program spełnia: MIMPI_Recv zwracać pierwszą (ze względu na czas przyjścia) wiadomość.
Test just_the_correct_msg.self sprawdza czy program odbiera wiadomość o wskazanej wielkości i wskazanym tagu.
Test lot_of_messages.self jest testem wydajnościowym, wysyłającym duże ilości wiadomości. Wartości zostały dobrane tak, żeby każde MIMPI_Recv zakończyło się sukcesem.