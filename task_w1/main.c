/*
Viet chuong trinh quan ly danh sach sinh vien bang danh sach lien ket don
Yeu cau:
    1. Nhap n sinh vien va them vao cuoi danh sach
    2. In danh sach sinh vien ra man hinh
    3. Tim sinh vien theo ma
    4. Xoa sinh vien theo ma
    5. Sap xep danh sach theo diem giam dan
    6. In ra danh sach sau khi xoa va sap xep
    7. Giai phong bo nho da cap phat
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SinhVien {
    char maSV[10];
    char hoTen[50];
    float diem;
} SinhVien;

typedef struct Node {
    SinhVien data;
    struct Node* next;
} Node;


Node* createNode (SinhVien sv) {
    Node* newNode = (Node*)malloc(sizeof(Node)); // Luu dia chi vung nho do malloc tra ve
    if (newNode == NULL) {
        return NULL;
    }
    newNode->data = sv;
    newNode->next = NULL;
    return newNode;
}

int nhapSoLuongSinhVien() {
    int n;
    printf("Nhap so luong sinh vien: ");
    if (scanf("%d", &n) != 1) {
        return 0;
    }
    return n;
}

int nhapMotSinhVien(SinhVien* sv, int stt) {
    printf("Nhap thong tin sinh vien thu %d:\n", stt);
    printf("Nhap ma sinh vien: ");
    if (scanf("%9s", sv->maSV) != 1) {
        return -1;
    }
    printf("Nhap ho ten: ");
    if (scanf(" %49[^\n]", sv->hoTen) != 1) {
        return -1;
    }
    printf("Nhap diem: ");
    if (scanf("%f", &sv->diem) != 1) {
        return -1;
    }
    return 0;
}

void xuatDanhSachLienKet(Node* head) {
    Node* temp = head;
    printf("Danh sach sinh vien (danh sach lien ket):\n");
    while (temp != NULL) {
        printf("Ma sinh vien: %s\n", temp->data.maSV);
        printf("Ho ten: %s\n", temp->data.hoTen);
        printf("Diem: %.2f\n", temp->data.diem);
        printf("-------------------------\n");
        temp = temp->next;
    }
}
int themSV (Node** head, SinhVien sv) {
    Node* newNode = createNode(sv);
    if (newNode == NULL) {
        return -1;
    }
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    return 0;
}

void giaiPhongDanhSach(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

int timMaSV (Node* head, const char* maSV) {
    Node* temp = head;
    while (temp != NULL) {  // Lặp qua node
        if (strcmp(temp->data.maSV, maSV) == 0) {
            return 1; 
        }
        temp = temp->next; // Đi qua các phần tử bằng con trở next 
    }
    return 0; 
}

int xoaSVTheoMa (Node** head, const char* maSV) {
    Node* temp = *head;
    Node* prev = NULL;

    while (temp != NULL) {
        if (strcmp(temp->data.maSV, maSV) == 0) {
            if (prev == NULL) {
                *head = temp->next; // Xoa node dau tien
            } else {
                prev->next = temp->next; // Xoa node giua hoac cuoi
            }
            free(temp);
            return 1; // Da xoa thanh cong
        }
        prev = temp;
        temp = temp->next;
    }
    return 0; // Khong tim thay sinh vien de xoa
}

int sapXepDanhSachTheoDiemGiamDan(Node** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return 0; // Danh sach rong hoac chi co mot phan tu
    }

    Node* sorted = NULL; // Danh sach lien ket da sap xep
    Node* current = *head;

    while (current != NULL) {
        Node* nextNode = current->next;
        if (sorted == NULL || current->data.diem > sorted->data.diem) {
            current->next = sorted;
            sorted = current;
        } else {
            Node* temp = sorted;
            while (temp->next != NULL && temp->next->data.diem >= current->data.diem) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = nextNode;
    }

    *head = sorted; // Cap nhat con tro head den danh sach da sap xep
    return 0; // Sap xep thanh cong
} 

int main() {
    int soLuong = nhapSoLuongSinhVien();
    if (soLuong <= 0) {
        printf("So luong sinh vien khong hop le.\n");
        return 1;
    }

    Node* head = NULL;  // con tro rong
    for (int i = 0; i < soLuong; i++) {
        SinhVien sv;
        if (nhapMotSinhVien(&sv, i + 1) != 0) {
            printf("Nhap du lieu khong hop le.\n");
            giaiPhongDanhSach(head);
            return 1;
        }

        if (themSV(&head, sv) != 0) {
            printf("Khong du bo nho de them sinh vien vao danh sach lien ket.\n");
            giaiPhongDanhSach(head);
            return 1;
        }
    }

    printf("\n");
    xuatDanhSachLienKet(head);
    printf("\n");

    char maCanTim [10];
    printf("Nhap ma sinh vien can tim: ");
    if (scanf("%9s", maCanTim) != 1) {
        printf("Nhap du lieu khong hop le.\n");
        giaiPhongDanhSach(head);
        return 1;
    }
    if (timMaSV(head, maCanTim)) {
        printf("Tim thay sinh vien co ma %s.\n", maCanTim);
    } else {
        printf("Khong tim thay sinh vien co ma %s.\n", maCanTim);
    }

    char maCanXoa [10];
    printf("Nhap ma sinh vien can xoa: ");
    if (scanf("%9s", maCanXoa) != 1) {
        printf("Nhap du lieu khong hop le.\n");
        giaiPhongDanhSach(head);
        return 1;
    }
    if (xoaSVTheoMa(&head, maCanXoa)) {
        printf("Da xoa sinh vien co ma %s.\n", maCanXoa);
    } else {
        printf("Khong tim thay sinh vien co ma %s.\n", maCanXoa);
    }

    printf("\nDanh sach sinh vien sau khi xoa:\n");
    xuatDanhSachLienKet(head);

    sapXepDanhSachTheoDiemGiamDan(&head);
    printf("\nDanh sach sinh vien sau khi sap xep theo diem giam dan:\n");
    xuatDanhSachLienKet(head);

    giaiPhongDanhSach(head);

    return 0;
}
