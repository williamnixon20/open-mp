# OPEN_MP

Program mencari invers matriks dengan menggunakan algoritma gauss-jordan, sesuai dengan program serial. Program melakukan parallelisasi dengan 4 proses dengan membagikan proses eliminasi ke N process yang ditentukan. 

## Cara Kerja Paralelisasi Program:
Secara garis besar, process 0 bertanggung jawab melakukan inisialisasi dan pembacaan matriks, melakukan scatter + gather, dan juga memprint hasil matriks identitas. Process lain akan memiliki matriks lokal hasil pembagian dengan MPI_SCATTER dan akan mengeliminasi pivot row dari matriks localnya sendiri. 

1. Proses 0 membaca dan menginisialisasi matriks pada sisi kanan dengan identitas.
2. Proses 0 menyiarkan ukuran matriks ke semua proses lainnya (MPI_BCAST).
3. Proses 0 membagi matriks menggunakan MPI_Scatter secara merata ke semua proses.
4. Setiap proses menginterasi semua baris matriks.
5. Setiap baris dibuat menjadi pivot dan dinormalisasi ke 1 oleh proses yang memiliki baris tersebut pada matriks lokalnya.
6. Baris pivot yang dinormalisasi disiarkan (B_CAST) ke setiap proses lainnya.
7. Proses lain yang tidak memiliki baris tersebut menunggu menerima baris pivot.
8. Setelah semua proses menerima baris pivot, mereka menghapus kolom yang sesuai dengan pivot yang dinormalisasi untuk membuat matriks awal menjadi identitas.
9. Setelah iterasi selesai, MPI_GATHER dilakukan untuk mengumpulkan setiap matriks lokal yang telah mengandung matriks identitas.
10. Process 0 melakukan printing matriks identitas ke file output.

## Cara Program Anda Membagikan Data Antar-Proses atau Antar-Thread dan Alasan Pemilihan Skema Pembagian Data:

Skema pembagian data yang dipilih adalah `Data Parallelism`. Skema ini dipilih karena memungkinkan pembagian data matriks yang ke semua proses, dengan harapan tidak akan ada process yang bekerja terlalu berat dan menjadi bottle neck. Data yang telah dibagikan tersebut kemudian diproses secara simultan dengan mengeliminasi matriks lokal. Akan tetapi, terdapat kelemahan pada kasus ini karena pemrosesan data tidak purely parallel, melainkan menunggu adanya BCAST dari pivot row pada setiap iterasi (proses masih harus menunggu pivot row jika row tersebut bukan merupakan tanggung jawabnya).

1. Data matrix dibagi menggunakan MPI_SCATTER dari proses 0 ke semua proses lainnya. Setelah itu, pivot row akan dibagikan oleh setiap process yang bertanggung jawab menormalisasi pivot row tersebut.
2. Setiap process bertanggung jawab pada matriks local yang telah dibagikan oleh MPI_SCATTER. 
4. Setiap iterasi akan memiliki row pivot yang akan di MPI_BCAST oleh process yang memiliki baris tersebut pada matriks yang telah di scatter.
3. Setelah iterasi selesai, semua process 0 akan mengumpulkan kembali matriks local yang telah dibagikan sebelumnya dengan MPI_GATHER. Matriks hasil yang digather ini memiliki matriks identitas pada bagian kiri matriks (matriks original) dan matriks invers pada bagian kanan matriks.

## Limitasi program
1. Tidak kasus matriks yang tidak memilik invers.
2. Testing hanya dilakukan dengan test cases, tidak diluar test case tersebut.
