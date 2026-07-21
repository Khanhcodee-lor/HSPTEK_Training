/*
Bai 2: Quan ly danh sach san pham bang danh sach lien ket don
Yeu cau:
1. Nhap n san pham va them vao cuoi danh sach.
2. In danh sach san pham.
3. Tim san pham theo ma.
4. Xoa san pham theo ma.
5. Sap xep danh sach theo don gia giam dan.
6. In danh sach sau khi xoa va sap xep.
7. Giai phong toan bo bo nho da cap phat.
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

// Tim kiem san pham theo ma truc tiep (khong dung con tro ham)
Node* timSanPhamTheoMa(Node* head, const char* maSP) {
	Node* temp = head;
	while (temp != NULL) {
		if (strcmp(temp->data.maSP, maSP) == 0) {
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

// Sap xep theo don gia giam dan truc tiep (khong dung con tro ham)
int sapXepDanhSachGiamDan(Node** head) {
	if (*head == NULL || (*head)->next == NULL) {
		return 0;
	}

	Node* sorted = NULL;
	Node* current = *head;

	while (current != NULL) {
		Node* nextNode = current->next;
		if (sorted == NULL || current->data.donGia > sorted->data.donGia) {
			current->next = sorted;
			sorted = current;
		} else {
			Node* temp = sorted;
			while (temp->next != NULL && temp->next->data.donGia >= current->data.donGia) {
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
	printf("Quan ly san pham bang danh sach lien ket\n");
	printf("Nhap so luong san pham: ");
	if (scanf("%d", &soLuong) != 1 || soLuong <= 0) {
		printf("So luong khong hop le.\n");
		return 1;
	}

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

	// TIM KIEM theo ma
	char maCanTim[10];
	printf("Nhap ma san pham can tim: ");
	if (scanf("%9s", maCanTim) != 1) {
		giaiPhongDanhSach(head);
		return 1;
	}
	Node* timThay = timSanPhamTheoMa(head, maCanTim);
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

	// SAP XEP
	printf("Sap xep theo gia giam dan...\n");
	sapXepDanhSachGiamDan(&head);
	xuatDanhSach(head);

	giaiPhongDanhSach(head);
	return 0;
}
