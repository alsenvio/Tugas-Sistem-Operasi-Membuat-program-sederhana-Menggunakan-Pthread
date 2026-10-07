#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>

// ============================================================
// STRUKTUR DATA REKENING BANK
// ============================================================
// BankAccount menyimpan:
// - saldo : saldo rekening
// - mutex : mutex untuk mengamankan akses ke saldo
typedef struct {
    int saldo;
    pthread_mutex_t mutex;
} BankAccount;


// ============================================================
// FUNGSI DEPOSIT
// ============================================================
// Menambahkan saldo ke rekening.
//
// pthread_mutex_lock() digunakan sebelum mengakses saldo
// karena bagian ini merupakan CRITICAL SECTION.
//
// pthread_mutex_unlock() digunakan setelah selesai mengakses
// saldo sehingga thread lain dapat menggunakan saldo.
void deposit(BankAccount *rekening, int jumlah)
{
    // Memulai Critical Section
    pthread_mutex_lock(&rekening->mutex);

    rekening->saldo += jumlah;

    printf("[DEPOSIT]  Thread menambahkan Rp%d | Saldo sekarang: Rp%d\n",
            jumlah, rekening->saldo);

    // Mengakhiri Critical Section
    pthread_mutex_unlock(&rekening->mutex);
}


// ============================================================
// FUNGSI WITHDRAW
// ============================================================
// Mengurangi saldo rekening jika saldo mencukupi.
//
// Pemeriksaan saldo dan pengurangan saldo harus dilakukan
// dalam satu Critical Section agar tidak terjadi Race Condition.
void withdraw(BankAccount *rekening, int jumlah)
{
    // Memulai Critical Section
    pthread_mutex_lock(&rekening->mutex);

    if (rekening->saldo >= jumlah)
    {
        rekening->saldo -= jumlah;

        printf("[WITHDRAW] Thread menarik Rp%d | Saldo sekarang: Rp%d\n",
                jumlah, rekening->saldo);
    }
    else
    {
        printf("[WITHDRAW] GAGAL menarik Rp%d | Saldo tidak mencukupi. "
                "Saldo: Rp%d\n",
                jumlah, rekening->saldo);
    }

    // Mengakhiri Critical Section
    pthread_mutex_unlock(&rekening->mutex);
}


// ============================================================
// FUNGSI TRANSFER OUT
// ============================================================
// Mengurangi saldo untuk simulasi transfer keluar.
//
// Sama seperti withdraw, pengecekan saldo dan pengurangan
// saldo harus diamankan menggunakan mutex.
void transferOut(BankAccount *rekening, int jumlah)
{
    // Memulai Critical Section
    pthread_mutex_lock(&rekening->mutex);

    if (rekening->saldo >= jumlah)
    {
        rekening->saldo -= jumlah;

        printf("[TRANSFER] Thread transfer keluar Rp%d | "
                "Saldo sekarang: Rp%d\n",
                jumlah, rekening->saldo);
    }
    else
    {
        printf("[TRANSFER] GAGAL transfer Rp%d | "
                "Saldo tidak mencukupi. Saldo: Rp%d\n",
                jumlah, rekening->saldo);
    }

    // Mengakhiri Critical Section
    pthread_mutex_unlock(&rekening->mutex);
}


// ============================================================
// THREAD 1 - DEPOSIT
// ============================================================
// Thread 1 memiliki tugas melakukan deposit.
//
// Deposit dilakukan sebanyak 3 kali dengan nominal Rp100.000.
// Setelah transaksi selesai, thread tetap aktif selama 30 detik
// agar dapat diamati melalui Task Manager.
void *threadDeposit(void *arg)
{
    BankAccount *rekening = (BankAccount *)arg;

    printf("\n[THREAD 1] Thread Deposit dimulai.\n");

    // Melakukan deposit sebanyak 3 kali
    for (int i = 1; i <= 3; i++)
    {
        printf("[THREAD 1] Transaksi deposit ke-%d\n", i);

        deposit(rekening, 100000);

        // Jeda 500 ms antar transaksi
        Sleep(500);
    }

    printf("[THREAD 1] tetap aktif selama 30 detik untuk pengamatan\n");

    // Membuat thread tetap aktif selama 30 detik
    Sleep(30000);

    printf("[THREAD 1] Selesai.\n");

    return NULL;
}


// ============================================================
// THREAD 2 - WITHDRAW
// ============================================================
// Thread 2 memiliki tugas melakukan penarikan.
//
// Penarikan dilakukan sebanyak 3 kali dengan nominal Rp50.000.
void *threadWithdraw(void *arg)
{
    BankAccount *rekening = (BankAccount *)arg;

    printf("\n[THREAD 2] Thread Withdraw dimulai.\n");

    // Melakukan withdraw sebanyak 3 kali
    for (int i = 1; i <= 3; i++)
    {
        printf("[THREAD 2] Transaksi withdraw ke-%d\n", i);

        withdraw(rekening, 50000);

        // Jeda 500 ms antar transaksi
        Sleep(500);
    }

    printf("[THREAD 2] tetap aktif selama 30 detik untuk pengamatan\n");

    // Membuat thread tetap aktif selama 30 detik
    Sleep(30000);

    printf("[THREAD 2] Selesai.\n");

    return NULL;
}


// ============================================================
// THREAD 3 - TRANSFER OUT
// ============================================================
// Thread 3 memiliki tugas melakukan transfer keluar.
//
// Transfer dilakukan sebanyak 3 kali dengan nominal Rp150.000.
void *threadTransfer(void *arg)
{
    BankAccount *rekening = (BankAccount *)arg;

    printf("\n[THREAD 3] Thread Transfer dimulai.\n");

    // Melakukan transfer sebanyak 3 kali
    for (int i = 1; i <= 3; i++)
    {
        printf("[THREAD 3] Transaksi transfer ke-%d\n", i);

        transferOut(rekening, 150000);

        // Jeda 500 ms antar transaksi
        Sleep(500);
    }

    printf("[THREAD 3] tetap aktif selama 30 detik untuk pengamatan\n");

    // Membuat thread tetap aktif selama 30 detik
    Sleep(30000);

    printf("[THREAD 3] Selesai.\n");

    return NULL;
}


// ============================================================
// FUNGSI MAIN
// ============================================================
int main()
{
    // Deklarasi rekening
    BankAccount rekening;

    // Deklarasi tiga thread
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    // ========================================================
    // INISIALISASI SALDO AWAL
    // ========================================================
    rekening.saldo = 1000000;

    printf("==============================================\n");
    printf("   SIMULASI SISTEM TRANSAKSI BANK\n");
    printf("   MULTITHREADING + MUTEX\n");
    printf("==============================================\n");

    printf("Saldo awal: Rp%d\n\n", rekening.saldo);


    // ========================================================
    // INISIALISASI MUTEX
    // ========================================================
    // Mutex digunakan untuk melakukan sinkronisasi akses
    // terhadap variabel saldo.
    //
    // Hanya satu thread yang boleh masuk ke Critical Section
    // pada satu waktu.
    if (pthread_mutex_init(&rekening.mutex, NULL) != 0)
    {
        printf("Gagal menginisialisasi mutex.\n");
        return 1;
    }

    printf("Mutex berhasil diinisialisasi.\n");


    // ========================================================
    // MEMBUAT THREAD
    // ========================================================
    // Ketiga thread menggunakan rekening yang sama.
    //
    // &rekening dikirim sebagai argumen sehingga Thread 1,
    // Thread 2, dan Thread 3 mengakses saldo yang sama.

    if (pthread_create(&thread1, NULL, threadDeposit, &rekening) != 0)
    {
        printf("Gagal membuat Thread 1.\n");
        pthread_mutex_destroy(&rekening.mutex);
        return 1;
    }

    if (pthread_create(&thread2, NULL, threadWithdraw, &rekening) != 0)
    {
        printf("Gagal membuat Thread 2.\n");
        pthread_join(thread1, NULL);
        pthread_mutex_destroy(&rekening.mutex);
        return 1;
    }

    if (pthread_create(&thread3, NULL, threadTransfer, &rekening) != 0)
    {
        printf("Gagal membuat Thread 3.\n");
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        pthread_mutex_destroy(&rekening.mutex);
        return 1;
    }


    // ========================================================
    // INSTRUKSI TASK MANAGER
    // ========================================================
    printf("\n==============================================\n");
    printf("Tiga thread sedang berjalan.\n");
    printf("Silakan buka Task Manager -> Details.\n");
    printf("Cari proses program ini untuk pengamatan.\n");
    printf("==============================================\n\n");


    // ========================================================
    // SYNCHRONISASI THREAD DENGAN PTHREAD_JOIN
    // ========================================================
    // pthread_join() membuat main thread menunggu sampai
    // masing-masing thread selesai.
    //
    // Tanpa pthread_join(), main() dapat selesai terlebih dahulu
    // sebelum ketiga thread selesai menjalankan tugasnya.

    printf("Menunggu ketiga thread selesai...\n");

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);


    // ========================================================
    // MENAMPILKAN SALDO AKHIR
    // ========================================================
    printf("\n==============================================\n");
    printf("SEMUA THREAD TELAH SELESAI\n");
    printf("Saldo akhir: Rp%d\n", rekening.saldo);
    printf("==============================================\n");


    // ========================================================
    // MENGHANCURKAN MUTEX
    // ========================================================
    // Mutex tidak lagi diperlukan setelah seluruh thread selesai.
    pthread_mutex_destroy(&rekening.mutex);

    printf("Mutex berhasil dihancurkan.\n");

    return 0;
}