/*
Bai 1: Quan ly danh sach sinh vien bang mang dong

Muc tieu:
- Luyen tap struct, mang, con tro, ham, malloc/free.

Yeu cau:
1. Nhap n sinh vien.
2. Luu du lieu bang mang dong cap phat boi malloc.
3. In danh sach sinh vien ra man hinh.
4. Tim sinh vien theo ma.
5. Sap xep danh sach theo diem giam dan.
6. Giai phong bo nho truoc khi ket thuc.

Goi y:
- Dung struct SinhVien gom: maSV, hoTen, diem.
- Viet cac ham rieng: nhapSinhVien, xuatSinhVien, timTheoMa, sapXepGiamDan.
- Co the dung strcmp de so sanh ma sinh vien.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct SinhVien {
	char maSV[10];
	char hoTen[50];
	float diem;
} SinhVien;

void nhapSinhVien(SinhVien* sv) {
    printf("Nhap ma sinh vien: ");
    scanf("%9s", sv->maSV);
    printf("Nhap ho ten: ");
    scanf(" %49[^\n]", sv->hoTen);
    printf("Nhap diem: ");
    scanf("%f", &sv->diem);
}

void xuatSinhVien(const SinhVien* sv) {
    printf("Ma SV: %s, Ho ten: %s, Diem: %.2f\n", sv->maSV, sv->hoTen, sv->diem);
}
void sapXepGiamDan(SinhVien* ds, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (ds[j].diem < ds[j + 1].diem) {
                SinhVien temp = ds[j];
                ds[j] = ds[j + 1];
                ds[j + 1] = temp;
            }
        }
    }
}

void giaiPhong(SinhVien* ds) {
    free(ds);
}
void timTheoMa(SinhVien* ds, int n, const char* maSV) {
    for (int i = 0; i < n; i++) {
        if (strcmp(ds[i].maSV, maSV) == 0) {
            printf("Tim thay sinh vien:\n");
            xuatSinhVien(&ds[i]);
            return;
        }
    }
    printf("Khong tim thay sinh vien co ma %s.\n", maSV);
}
int main() {
    int n;
    printf("Nhap so luong sinh vien: ");
    scanf("%d", &n);

    SinhVien* ds = (SinhVien*)malloc(n * sizeof(SinhVien));
    if (ds == NULL) {
        printf("Khong du bo nho de cap phat.\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        printf("Nhap thong tin sinh vien thu %d:\n", i + 1);
        nhapSinhVien(&ds[i]);
    }

    printf("\nDanh sach sinh vien:\n");
    for (int i = 0; i < n; i++) {
        xuatSinhVien(&ds[i]);
    }

    char maCanTim[10];
    printf("\nNhap ma sinh vien can tim: ");
    scanf("%9s", maCanTim);
    timTheoMa(ds, n, maCanTim);

    sapXepGiamDan(ds, n);
    printf("\nDanh sach sinh vien sau khi sap xep theo diem giam dan:\n");
    for (int i = 0; i < n; i++) {
        xuatSinhVien(&ds[i]);
    }

    giaiPhong(ds);
    return 0;
}
