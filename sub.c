#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ALPHABET_SIZE 26
#define MAX_TEXT_LEN 1024
#define MAX_WORD_LEN 50
#define MAX_DICT_WORDS 30000

// 全局变量
char ciphertext[MAX_TEXT_LEN] = "";
char decrypted[MAX_TEXT_LEN] = "";
char key[ALPHABET_SIZE + 1] = "??????????????????????????"; // 初始未知密钥
char freq_order[ALPHABET_SIZE + 1] = "etaoinshrdlcumwfgypbvkjxqz";
const char *common_bigrams[] = {"th", "he", "in", "er", "an", "re", "on", "at", "en", "nd", NULL};

// 函数声明
void encrypt();
void decrypt();
void crack_cipher();
void frequency_analysis();
void bigram_analysis();
void dictionary_suggestions();
void update_decryption();
void update_key(char cipher_char, char plain_char);
void reset_key();
void display_current_state();
int load_dictionary(const char *filename, char dict[][MAX_WORD_LEN]);
void str_tolower(char *str);

int main() {
    int choice;
    
    printf("=== 单表代换密码系统 ===\n");
    
    while(1) {
        printf("\n主菜单:\n");
        printf("1. 加密文本\n");
        printf("2. 解密文本\n");
        printf("3. 破译密文\n");
        printf("4. 退出\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar(); // 消耗换行符
        
        switch(choice) {
            case 1:
                encrypt();
                break;
            case 2:
                decrypt();
                break;
            case 3:
                crack_cipher();
                break;
            case 4:
                exit(0);
            default:
                printf("无效选择!\n");
        }
    }
    
    return 0;
}

void encrypt() {
    char plaintext[MAX_TEXT_LEN];
    char custom_key[ALPHABET_SIZE + 1];
    
    printf("\n输入要加密的文本 (仅字母): ");
    fgets(plaintext, MAX_TEXT_LEN, stdin);
    plaintext[strcspn(plaintext, "\n")] = '\0'; // 移除换行符
    
    printf("输入26个字母的密钥 (小写): ");
    fgets(custom_key, ALPHABET_SIZE + 1, stdin);
    getchar(); // 消耗换行符
    
    // 转换为小写
    str_tolower(plaintext);
    str_tolower(custom_key);
    
    // 加密过程
    printf("\n加密结果: ");
    for(int i = 0; plaintext[i]; i++) {
        if(isalpha(plaintext[i])) {
            char encrypted_char = custom_key[plaintext[i] - 'a'];
            putchar(encrypted_char);
        } else {
            putchar(plaintext[i]);
        }
    }
    printf("\n");
}

void decrypt() {
    char cipher[MAX_TEXT_LEN];
    char custom_key[ALPHABET_SIZE + 1];
    
    printf("\n输入要解密的文本: ");
    fgets(cipher, MAX_TEXT_LEN, stdin);
    cipher[strcspn(cipher, "\n")] = '\0';
    
    printf("输入26个字母的密钥 (小写): ");
    fgets(custom_key, ALPHABET_SIZE + 1, stdin);
    getchar();
    
    str_tolower(cipher);
    str_tolower(custom_key);
    
    // 创建反向映射
    char reverse_key[ALPHABET_SIZE];
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        reverse_key[custom_key[i] - 'a'] = 'a' + i;
    }
    
    // 解密过程
    printf("\n解密结果: ");
    for(int i = 0; cipher[i]; i++) {
        if(isalpha(cipher[i])) {
            char decrypted_char = reverse_key[cipher[i] - 'a'];
            putchar(decrypted_char);
        } else {
            putchar(cipher[i]);
        }
    }
    printf("\n");
}

void crack_cipher() {
    printf("\n输入要破译的密文: ");
    fgets(ciphertext, MAX_TEXT_LEN, stdin);
    ciphertext[strcspn(ciphertext, "\n")] = '\0';
    str_tolower(ciphertext);
    
    reset_key();
    update_decryption();
    
    int choice;
    char input[10];
    
    while(1) {
        display_current_state();
        
        printf("\n破译菜单:\n");
        printf("1. 显示频率分析\n");
        printf("2. 显示字母连接分析\n");
        printf("3. 显示字典建议\n");
        printf("4. 指定密钥映射\n");
        printf("5. 重置密钥\n");
        printf("6. 返回主菜单\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar();
        
        switch(choice) {
            case 1:
                frequency_analysis();
                break;
            case 2:
                bigram_analysis();
                break;
            case 3:
                dictionary_suggestions();
                break;
            case 4:
                printf("输入映射 (格式: 密文字母=明文字母): ");
                fgets(input, 10, stdin);
                if(strlen(input) >= 3 && input[1] == '=') {
                    char cipher_char = tolower(input[0]);
                    char plain_char = tolower(input[2]);
                    if(isalpha(cipher_char) && isalpha(plain_char)) {
                        update_key(cipher_char, plain_char);
                        update_decryption();
                    }
                }
                break;
            case 5:
                reset_key();
                update_decryption();
                break;
            case 6:
                return;
            default:
                printf("无效选择!\n");
        }
    }
}

void frequency_analysis() {
    int counts[ALPHABET_SIZE] = {0};
    int total_letters = 0;
    
    // 统计字母频率
    for(int i = 0; ciphertext[i]; i++) {
        if(isalpha(ciphertext[i])) {
            counts[ciphertext[i] - 'a']++;
            total_letters++;
        }
    }
    
    // 创建频率排序的字母数组
    char freq_sorted[ALPHABET_SIZE + 1] = {0};
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        int max_idx = 0;
        for(int j = 0; j < ALPHABET_SIZE; j++) {
            if(counts[j] > counts[max_idx]) {
                max_idx = j;
            }
        }
        freq_sorted[i] = 'a' + max_idx;
        counts[max_idx] = -1; // 标记为已处理
    }
    freq_sorted[ALPHABET_SIZE] = '\0';
    
    printf("\n字母频率统计 (降序):\n");
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        if(freq_sorted[i] >= 'a') {
            printf("%c ", freq_sorted[i]);
        }
    }
    
    printf("\n\n建议映射:\n");
    printf("密文字母 -> 明文字母 (基于英文频率)\n");
    printf("----------------------------------\n");
    for(int i = 0; i < ALPHABET_SIZE && freq_order[i]; i++) {
        if(freq_sorted[i] >= 'a') {
            printf("   %c      ->    %c   \n", freq_sorted[i], freq_order[i]);
        }
    }
}

void bigram_analysis() {
    char bigrams[100][3] = {0};
    int counts[100] = {0};
    int bigram_count = 0;
    
    // 收集双字母组合
    for(int i = 0; ciphertext[i] && ciphertext[i+1]; i++) {
        if(isalpha(ciphertext[i]) && isalpha(ciphertext[i+1])) {
            char bigram[3] = {ciphertext[i], ciphertext[i+1], '\0'};
            
            int found = 0;
            for(int j = 0; j < bigram_count; j++) {
                if(strcmp(bigrams[j], bigram) == 0) {
                    counts[j]++;
                    found = 1;
                    break;
                }
            }
            
            if(!found) {
                strcpy(bigrams[bigram_count], bigram);
                counts[bigram_count] = 1;
                bigram_count++;
            }
        }
    }
    
    // 排序双字母组合
    for(int i = 0; i < bigram_count - 1; i++) {
        for(int j = i+1; j < bigram_count; j++) {
            if(counts[j] > counts[i]) {
                int temp_count = counts[i];
                counts[i] = counts[j];
                counts[j] = temp_count;
                
                char temp_bigram[3];
                strcpy(temp_bigram, bigrams[i]);
                strcpy(bigrams[i], bigrams[j]);
                strcpy(bigrams[j], temp_bigram);
            }
        }
    }
    
    printf("\n常见双字母组合 (前10个):\n");
    printf("----------------------\n");
    int display_count = bigram_count > 10 ? 10 : bigram_count;
    for(int i = 0; i < display_count; i++) {
        printf("%s: %d次\n", bigrams[i], counts[i]);
    }
    
    printf("\n建议映射 (基于常见双字母):\n");
    printf("------------------------------\n");
    for(int i = 0; common_bigrams[i] != NULL && i < 5; i++) {
        printf("如果明文包含 \"%s\", 可能映射为:\n", common_bigrams[i]);
        for(int j = 0; j < display_count; j++) {
            printf("  %s -> %s\n", bigrams[j], common_bigrams[i]);
        }
    }
}

void dictionary_suggestions() {
    static char dict[MAX_DICT_WORDS][MAX_WORD_LEN];
    static int loaded = 0;
    static int dict_size = 0;
    
    if(!loaded) {
        dict_size = load_dictionary("words.txt", dict);
        if(dict_size == 0) {
            printf("字典文件未找到! 请创建包含单词的words.txt文件\n");
            return;
        }
        loaded = 1;
    }
    
    // 提取密文中的单词
    char words[100][MAX_WORD_LEN] = {0};
    int word_count = 0;
    char *token = strtok(ciphertext, " ,.!?;:\"\n\t");
    
    while(token != NULL && word_count < 100) {
        if(strlen(token) > 2) { // 只考虑长度大于2的单词
            strcpy(words[word_count], token);
            word_count++;
        }
        token = strtok(NULL, " ,.!?;:\"\n\t");
    }
    
    printf("\n基于字典的建议:\n");
    printf("----------------\n");
    
    int suggestions = 0;
    for(int i = 0; i < word_count && suggestions < 5; i++) {
        char pattern[MAX_WORD_LEN] = {0};
        int unknown_count = 0;
        
        // 创建模式 (已知字母显示，未知用?)
        for(int j = 0; words[i][j]; j++) {
            if(isalpha(words[i][j])) {
                if(key[words[i][j] - 'a'] != '?') {
                    pattern[j] = key[words[i][j] - 'a'];
                } else {
                    pattern[j] = '?';
                    unknown_count++;
                }
            } else {
                pattern[j] = words[i][j];
            }
        }
        
        // 查找匹配
        if(unknown_count > 0 && unknown_count < 3) { // 只显示少量未知字母的单词
            printf("\n单词: %s\n", words[i]);
            printf("当前模式: %s\n", pattern);
            printf("可能的匹配:\n");
            
            for(int k = 0; k < dict_size; k++) {
                if(strlen(dict[k]) != strlen(pattern)) continue;
                
                int match = 1;
                for(int m = 0; pattern[m]; m++) {
                    if(pattern[m] != '?' && pattern[m] != dict[k][m]) {
                        match = 0;
                        break;
                    }
                }
                
                if(match) {
                    printf("  %s\n", dict[k]);
                    suggestions++;
                }
            }
        }
    }
    
    if(suggestions == 0) {
        printf("未找到建议。尝试指定更多字母映射。\n");
    }
}

void update_decryption() {
    strcpy(decrypted, ciphertext);
    
    for(int i = 0; decrypted[i]; i++) {
        if(isalpha(decrypted[i])) {
            if(key[decrypted[i] - 'a'] != '?') {
                decrypted[i] = key[decrypted[i] - 'a'];
            } else {
                decrypted[i] = toupper(decrypted[i]); // 未知字母大写显示
            }
        }
    }
}

void update_key(char cipher_char, char plain_char) {
    // 清除该明文字母的现有映射
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        if(key[i] == plain_char) {
            key[i] = '?';
        }
    }
    
    // 设置新映射
    key[cipher_char - 'a'] = plain_char;
    printf("更新映射: %c -> %c\n", cipher_char, plain_char);
}

void reset_key() {
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        key[i] = '?';
    }
    key[ALPHABET_SIZE] = '\0';
}

void display_current_state() {
    printf("\n当前密钥映射:\n");
    printf("密文字母: ");
    for(char c = 'a'; c <= 'z'; c++) {
        printf("%c ", c);
    }
    printf("\n明文字母: ");
    for(int i = 0; i < ALPHABET_SIZE; i++) {
        printf("%c ", key[i]);
    }
    
    printf("\n\n当前解密文本:\n%s\n", decrypted);
}

int load_dictionary(const char *filename, char dict[][MAX_WORD_LEN]) {
    FILE *file = fopen(filename, "r");
    if(!file) return 0;
    
    int count = 0;
    while(fgets(dict[count], MAX_WORD_LEN, file) && count < MAX_DICT_WORDS) {
        dict[count][strcspn(dict[count], "\n")] = '\0';
        str_tolower(dict[count]);
        count++;
    }
    
    fclose(file);
    return count;
}

void str_tolower(char *str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}
