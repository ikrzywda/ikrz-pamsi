# Algorytmy sortowania

> Igor Krzywda

Program zawiera trzy algorytmy sortowania: quicksort, mergesort oraz bucketsort. Sortowanie odbywa się na datasecie ocen filmów.

## Kompilacja i uruchomienie

```sh
make    # kompilacja
./build/bin/program <plik> <indeks kolumny*> <liczba recorów> <algorytm> <liczba przejść randomizacji> <rand seed>  # wywołanie pliku wynonywalnego
python benchmark.py # wywołanie przeprowadzenia benchmarków dla kazdego algorytmu
```

Indeks kolumny jest placeholderem, wybór kolumny opiera się o wskazanie headera pliku CSV, wybierany jest zawsze `rating`.

## Quicksort

### Opis

Quicksort jset algorytmem "divide and conquer", polega na wyborze pivota, który jest elementem listy i podzieleniu mienionej listy na dwie podlisty, odpowiednio z mniejszymi i większymi elementami od pivota. W programie quicksort jest zaimplementowany iteracyjnie, aby uniknąć stack overflow spowodowany zbyt duzą liczbą elementów w call stacku rekurencyjnych wywołań.

### Złożoności względem długości listy

- **najgorszy przypadek** $\mathcal{O}(N^2)$
- **średni przypadek** $\mathcal{O}(Nlog(N)$
- **najlepszy przypadek** $\mathcal{O}(Nlog(N)$

## Mergesort

### Opis

Mergesort jest algorytmem "divide and conquer", polega na rekurencyjnym dzieleniu sortowanej listy na pół, sortowaniu podlist i ponownym ich złączeniu. Przyjmuje się, że lista jednoelementowa jest posortowana.

### Złożoności względem długości listy

- **najgorszy przypadek** $\mathcal{O}(Nlog(N))$
- **średni przypadek** $\mathcal{\Omega}(Nlog(N)$
- **najlepszy przypadek** $\mathcal{\Theta}(Nlog(N)$

## Bucketsort

### Opis

Bucketsort polega na podzieleniu sortowanej listy na "buckety", które są podlistami zawierające wartości z ustalonego przedziału. Po podzieleniu listy na "buckety", dla każdej podlisty aplikowany jest osobny algorytm sortowania (w opisywanej implementacji jest użyty quicksort).

### Złożoności względem długości listy (N) oraz liczby bucketów (K)

- **najgorszy przypadek** $\mathcal{O}(N^2)$
- **średni przypadek** $\mathcal{O}(N + \frac{Nlog(N)}{K} + K)$
- **najlepszy przypadek** $\mathcal{O}(N + \frac{Nlog(N)}{K} + K)$

## Eksperymenty

Eksperymenty były przeprowadzane poprzez wykonywanie programu na danych o liczbie recordów odpowiednio 10_000, 100_000, 500_000 oraz 1_000_000.

### Wyniki

#### Mergesort

| Liczba recorów | Średnia wyników | Mediana wyników | Czas wykonania (ms) |
| -------------- | --------------- | --------------- | ------------------- |
| 10000          | 5.4603          | 5.0             | 452                 |
| 100000         | 6.08993         | 7.0             | 3190                |
| 500000         | 6.66572         | 7.0             | 16493               |
| 1000000        | 6.5934          | 7.0             | 37605               |

![Mergesort Results](assets/benchmark_plot_mergesort.png)

#### Quicksort

| Liczba recorów | Średnia wyników | Mediana wyników | Czas wykonania (ms) |
| -------------- | --------------- | --------------- | ------------------- |
| 10000          | 5.4603          | 5.0             | 195                 |
| 100000         | 6.08993         | 7.0             | 6156                |
| 500000         | 6.66572         | 7.0             | 134996              |
| 1000000        | 6.5934          | 7.0             | 475145              |

![Mergesort Results](assets/benchmark_plot_quicksort.png)

#### Bucketsort

| Liczba recorów | Średnia wyników | Mediana wyników | Czas wykonania (ms) |
| -------------- | --------------- | --------------- | ------------------- |
| 10000          | 5.4603          | 5.0             | 199                 |
| 100000         | 6.08993         | 7.0             | 6236                |
| 500000         | 6.66572         | 7.0             | 135112              |
| 1000000        | 6.5934          | 7.0             | 475423              |

![Mergesort Results](assets/benchmark_plot_bucketsort.png)

### Wnioski

Wyniki eksperymentów w ogólności zgadzają się z teoretycznymi załozeniami przy średnim przypadku. Pokazuje to, że używanie losowego pivota w quicksorcie pozwala uniknąć najgorszego przypadku $\mathcal{O}(N)$.
Bucketsort i quicksort mają bardzo zbliżone realistyczne złożoności, co pokazuje, że wcześniejsze rozdzielanie danych nie ma większego wpływu i quicksort zajmuje podobną ilość czasu "rozdystrybuowany" na parę bucketów i na pełnej liście. Mergesort jest najszybszy ze wszystkich algorytmów dla większych zbiorów danych; operacja rozdzielania listy jest kosztowna, co widać dla najmniejszego zbioru danych, ale ten koszt się gubi przy większych zbiorach.
Dużym problemem przy implementacji, który spowalnia operacje jest czas wykonywania pojedynczych operacji, powodem jest nieumiejętne wykorzystywanie biblioteki STL, optymalizacja tych operacji daje najwięcej przestrzeni na poprawę.

### Bibliografia i narzędzia

- Github Copilot w trybie czatu

- Wikipedia contributors. (n.d). Merge sort. In Wikipedia, The Free Encyclopedia. 4.05.2024, from https://en.wikipedia.org/wiki/Merge_sort
- Wikipedia contributors. (n.d). Bucket sort. In Wikipedia, The Free Encyclopedia. 4.05.2024, from https://en.wikipedia.org/wiki/Bucket_sort
- Wikipedia contributors. (n.d). Quicksort. In Wikipedia, The Free Encyclopedia. 4.05.2024, from https://en.wikipedia.org/wiki/Quicksort
- Boule Thao. (n.d.). Algorithms in Python: Quicksort. Gist. Retrieved 4.05.2024, from https://gist.github.com/boulethao/ef29e7e5e19da82c5d64031aa30e2bde
- Leimao. (n.d). "C++ Traits.". Retrieved 4.05.2024. from https://leimao.github.io/blog/CPP-Traits/
