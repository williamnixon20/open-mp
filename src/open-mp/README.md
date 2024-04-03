# OPEN_MP

Program ini mencari invers matriks menggunakan algoritma Gauss-Jordan dengan paralelisasi menggunakan OpenMP (thread-based parallelism), yang membagi proses eliminasi ke dalam beberapa thread.

## Cara Kerja Paralelisasi Program:
Proses paralelisasi menggunakan OpenMP adalah sebagai berikut:

1. Program membaca dan menginisialisasi matriks pada sisi kanan dengan identitas.
2. Setiap iterasi akan dilakukan normalisasi dan eliminasi baris pivot menggunakan paralelisasi OpenMP.
3. Pada awal setiap iterasi, program memilih indeks pivot dengan menghitung offset. Jika nilai pivotnya mendekati nol, proses normalisasi dilewati karena baris tersebut sudah dinormalisasi sebelumnya. Namun, jika nilai pivot tidak mendekati nol, maka setiap elemen kolom pada baris pivot dibagi dengan nilai pivotnya. Paralelisasi dilakukan pada tahap ini menggunakan `#pragma omp parallel for`.
4. Setelah normalisasi, eliminasi dilakukan untuk setiap kolom pada matriks kecuali kolom pivot. Proses eliminasi juga dapat diparalelkan menggunakan `#pragma omp parallel for`.
5. Setelah semua operasi selesai maka akan disimpan hasil matriksnya.

## Cara Program Anda Membagikan Data Antar-Proses atau Antar-Thread dan Alasan Pemilihan Skema Pembagian Data:

Skema pembagian data yang dipilih adalah `Task Parallelism`. Skema ini didasarkan pada karakteristik operasi yang dilakukan pada matriks, di mana setiap iterasi dari algoritma Gaussian elimination dapat dieksekusi secara independen (pembagian nilai kolom pada sebuah baris dengan pivotnya dan proses eliminasi baris).

1. Data matriks dibagi di antara thread-thread yang tersedia pada saat program dijalankan oleh OpenMP. Thread akan secara otomatis dibuat dan dibagi beban kerja sesuai dengan jumlah yang tersedia.
2. Paralelisasi dengan OpenMP memungkinkan setiap thread untuk bekerja secara independen memproses normalisasi dan eliminasi baris pivot pada matriks. Hal ini akan memaksimalkan sumber daya komputasi yang ada pada sistem.

## Limitasi program
1. Tidak kasus matriks yang tidak memilik invers.
2. Testing hanya dilakukan dengan test cases, tidak diluar test case tersebut.
