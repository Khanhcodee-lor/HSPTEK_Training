/*
Bai 2: Quan ly danh sach san pham bang danh sach lien ket don
Dung CON TRO HAM (function pointer) cho sap xep va tim kiem

Muc tieu:
- Luyen tap con tro, struct, danh sach lien ket, cap phat dong, con tro ham.

Yeu cau:
1. Nhap n san pham va them vao cuoi danh sach.
2. In danh sach san pham.
3. Tim san pham theo ma (dung con tro ham).
4. Xoa san pham theo ma.
5. Sap xep danh sach theo don gia giam dan (dung con tro ham).
6. In danh sach sau khi xoa va sap xep.
7. Giai phong toan bo bo nho da cap phat.

Goi y - Con tro ham:
- typedef int (*CompareFunc)(float a, float b);  // So sanh gia
- typedef int (*SearchFunc)(SanPham sp, const char* ma);  // Tim kiem
- Truyen ham vao sapXepGiamDan va timTheoMa
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SanPham {
	char maSP[10];
	char tenSP[50];
	float donGia;
} SanPham;

typedef struct Node {
	SanPham data;
	struct Node* next;
} Node;

// ===== CON TRO HAM =====
typedef int (*CompareFunc)(float a, float b);  // Tra ve 1 neu a > b, 0 neu bang, -1 neu a < b
typedef int (*SearchFunc)(SanPham sp, const char* criteria);  // Tra ve 1 neu khop, 0 neu khong

// ===== STRUCT CHUA CON TRO HAM =====
typedef struct {
	CompareFunc compare;  // Con tro ham so sanh
	SearchFunc search;    // Con tro ham tim kiem
} Callbacks;  // Struct chua cac ham callback

// ===== HAM SO SANH =====
int compareGiamDan(float a, float b) {
	if (a > b) return 1;
	if (a < b) return -1;
	return 0;
}

int compareTangDan(float a, float b) {
	if (a < b) return 1;
	if (a > b) return -1;
	return 0;
}

// ===== HAM TIM KIEM =====
int searchByMa(SanPham sp, const char* ma) {
	return strcmp(sp.maSP, ma) == 0;
}

int searchByName(SanPham sp, const char* name) {
	return strstr(sp.tenSP, name) != NULL;
}

// ===== CAC HAM CHINH =====
Node* createNode(SanPham sp) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL) {
		return NULL;
	}
	newNode->data = sp;
	newNode->next = NULL;
	return newNode;
}

int nhapMotSanPham(SanPham* sp, int stt) {
	printf("Nhap thong tin san pham thu %d:\n", stt);
	printf("Ma san pham: ");
	if (scanf("%9s", sp->maSP) != 1) return -1;
	printf("Ten san pham: ");
	if (scanf(" %49[^\n]", sp->tenSP) != 1) return -1;
	printf("Don gia: ");
	if (scanf("%f", &sp->donGia) != 1) return -1;
	return 0;
}

int themSP(Node** head, SanPham sp) {
	Node* newNode = createNode(sp);
	if (newNode == NULL) return -1;
	
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

void xuatDanhSach(Node* head) {
	Node* temp = head;
	printf("Danh sach san pham:\n");
	while (temp != NULL) {
		printf("Ma: %s | Ten: %s | Gia: %.2f\n", 
		       temp->data.maSP, temp->data.tenSP, temp->data.donGia);
		temp = temp->next;
	}
	printf("---\n");
}

// Tim san pham dung CON TRO HAM (tu struct Callbacks)
Node* timSanPham(Node* head, const char* criteria, Callbacks callbacks) {
	Node* temp = head;
	while (temp != NULL) {
		if (callbacks.search(temp->data, criteria)) {  // Goi ham tu struct
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

int xoaSPTheoMa(Node** head, const char* maSP) {
	Node* temp = *head;
	Node* prev = NULL;
	
	while (temp != NULL) {
		if (strcmp(temp->data.maSP, maSP) == 0) {
			if (prev == NULL) {
				*head = temp->next;
			} else {
				prev->next = temp->next;
			}
			free(temp);
			return 1;
		}
		prev = temp;
		temp = temp->next;
	}
	return 0;
}

// Sap xep dung CON TRO HAM (tu struct Callbacks)
int sapXepDanhSach(Node** head, Callbacks callbacks) {
	if (*head == NULL || (*head)->next == NULL) {
		return 0;
	}

	Node* sorted = NULL;
	Node* current = *head;

	while (current != NULL) {
		Node* nextNode = current->next;
		if (sorted == NULL || callbacks.compare(current->data.donGia, sorted->data.donGia) > 0) {
			current->next = sorted;
			sorted = current;
		} else {
			Node* temp = sorted;
			while (temp->next != NULL && callbacks.compare(temp->next->data.donGia, current->data.donGia) >= 0) {
				temp = temp->next;
			}
			current->next = temp->next;
			temp->next = current;
		}
		current = nextNode;
	}

	*head = sorted;
	return 0;
}

void giaiPhongDanhSach(Node* head) {
	while (head != NULL) {
		Node* temp = head;
		head = head->next;
		free(temp);
	}
}

int main() {
	int soLuong;
	printf("Quan ly san pham bang danh sach lien ket (dung CON TRO HAM trong STRUCT)\n");
	printf("Nhap so luong san pham: ");
	if (scanf("%d", &soLuong) != 1 || soLuong <= 0) {
		printf("So luong khong hop le.\n");
		return 1;
	}

	// ===== KHOI TAO STRUCT CALLBACKS =====
	Callbacks callbacks;
	callbacks.compare = compareGiamDan;    // Gan ham so sanh vao struct
	callbacks.search = searchByMa;          // Gan ham tim kiem vao struct

	Node* head = NULL;
	for (int i = 0; i < soLuong; i++) {
		SanPham sp;
		if (nhapMotSanPham(&sp, i + 1) != 0) {
			printf("Nhap du lieu khong hop le.\n");
			giaiPhongDanhSach(head);
			return 1;
		}
		if (themSP(&head, sp) != 0) {
			printf("Khong du bo nho.\n");
			giaiPhongDanhSach(head);
			return 1;
		}
	}

	printf("\n");
	xuatDanhSach(head);

	// TIM KIEM dung CON TRO HAM trong struct
	char maCanTim[10];
	printf("Nhap ma san pham can tim: ");
	if (scanf("%9s", maCanTim) != 1) {
		giaiPhongDanhSach(head);
		return 1;
	}
	Node* timThay = timSanPham(head, maCanTim, callbacks);  // Truyen callbacks struct
	if (timThay != NULL) {
		printf("Tim thay: %s - %s (%.2f)\n", 
		       timThay->data.maSP, timThay->data.tenSP, timThay->data.donGia);
	} else {
		printf("Khong tim thay.\n");
	}

	// XOA san pham
	char maCanXoa[10];
	printf("Nhap ma san pham can xoa: ");
	if (scanf("%9s", maCanXoa) != 1) {
		giaiPhongDanhSach(head);
		return 1;
	}
	if (xoaSPTheoMa(&head, maCanXoa)) {
		printf("Da xoa.\n");
	} else {
		printf("Khong tim thay de xoa.\n");
	}

	printf("\nDanh sach sau khi xoa:\n");
	xuatDanhSach(head);

	// SAP XEP dung CON TRO HAM trong struct
	printf("Sap xep theo gia giam dan...\n");
	sapXepDanhSach(&head, callbacks);  // Truyen callbacks struct
	xuatDanhSach(head);

	giaiPhongDanhSach(head);
	return 0;
}
