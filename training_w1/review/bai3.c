/*
Đề bài: Viết hàm xóa Node đầu tiên của Danh sách liên kết

Kiểm tra xem danh sách có rỗng không (nếu *head == NULL thì không làm gì cả).
Nếu không rỗng, dùng một biến tạm Node* temp để lưu lại Node đầu tiên (tức là *head).
Cập nhật lại *head trỏ sang Node thứ hai (tức là temp->next).
Giải phóng bộ nhớ của Node đầu tiên cũ bằng lệnh free(temp).
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next; // một biến con  trỏ để lưu địa chỉ ôn nhớ
}Node;

void xoaDau (Node** head){ // Địa chỉ của biến head
    if (*head == NULL) return;
    else {
        Node* temp = *head; // temp trỏ đến head (lay dia chi)
        *head = temp->next; 
        free(temp);
    }
}

void inDanhSach (Node* head) {
    Node* temp = head;
    while (temp != NULL){
        printf("%d ", temp->data); 
        temp = temp->next;
    }
    printf("\n");
}


int main(){
    Node* head = (Node*)malloc(sizeof(Node));
    Node* second = (Node*)malloc(sizeof(Node));
    Node* third = (Node*)malloc(sizeof(Node));

    head->data = 10; //Con trỏ head đi đến vùng nhớ của node đầu tiên mà nó đang quản lý, truy cập vào phần dữ liệu của node đó và gán giá trị bằng 10
    head->next = second;  // dung head gan cho next dia chi o nho cua second.

    second->data = 20;
    second->next = third;

    third->data = 30;
    third->next = NULL;

    printf("Danh sach ban dau: ");
    inDanhSach(head);
    
    xoaDau(&head);
    printf("Danh sach sau khi xoa: ");
    inDanhSach(head);
}