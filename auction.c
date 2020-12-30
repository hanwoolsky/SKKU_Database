#include <stdio.h>
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//#include <my_global.h>  // this is not needed in 8.0

MYSQL *conn;
MYSQL_RES *result;
MYSQL_ROW row;
int MainMenu(char *email);

int DateCheck(char *curdate){
    int query;
    int query1;
    int query2;
    int query3;
    int cond;
    int cur;
    long int date_num = 0;
    long int curdate_num = 0;
    char date[20];
    char seller[20];
    char Date_query[200];
    char Infoquery[200];
    char Tradequery[300];
    char Sellquery[50];

    cur = strlen(curdate);
    for (int i = 0; i < cur; i++){
        if((curdate[i] != '-') && (curdate[i] != ':') && (curdate[i] != ' ')){
            curdate_num = curdate[i] - '0' + curdate_num * 10;
        }
    }

    sprintf(Date_query, "select item_id, bid_ending_date from item_info where bid_ending_date > 0 and pur_status = 'Valid';");
    query = mysql_query(conn, Date_query);
    if (query != 0) {
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    while (row = mysql_fetch_row(result)){
        cond = atoi(row[0]);
        sprintf(date, "%s", row[1]);
        int count = strlen(date);
        date_num = 0;
        for (int i = 0; i < count; i++){
            if((date[i] != '-') && (date[i] != ':') && (date[i] != ' ')){
                date_num = date[i] - '0' + date_num * 10;
            }
        }
        if (curdate_num - date_num > 0){
            sprintf(Infoquery, "UPDATE item_info SET pur_status = 'Sold' where item_id = %d;", cond);
            query1 = mysql_query(conn, Infoquery);
            if (query1 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            sprintf(Sellquery, "select seller from item where item_id = '%d';", cond);
            query3 = mysql_query(conn, Sellquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result1 = mysql_store_result(conn);
            row = mysql_fetch_row(result1);
            sprintf(seller, "%s", row[0]);
            sprintf(Tradequery, "insert ignore into trade value(NULL, '%s', '%d', 0, NULL);", seller, cond); 
            query2 = mysql_query(conn, Tradequery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
    }
    mysql_free_result(result);
}

int Bid_Ending(int cond){
    int query;
    int query2;
    char date[20];
    char Date_query[200];
    char bidhiquery[200];
    char posted_date[20];
    struct tm *t;
    time_t timer;

    timer = time(NULL);
    t = localtime(&timer);
    sprintf(posted_date, "%04d-%02d-%02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    
    printf("---- bid ending date (yyyy-mm-dd HH:mm, e.g. 2020-12-04 23:59) : ");
    scanf(" %[^\n]s", date);

    sprintf(Date_query, "UPDATE item_info SET posted_date = '%s', bid_ending_date = '%s' where item_id = %d;", posted_date, date, cond);
    sprintf(bidhiquery, "insert into bid_history values ('%d', NULL, '%s', 0);", cond, posted_date);
    query = mysql_query(conn, Date_query);
    query2 = mysql_query(conn, bidhiquery);
    if ((query != 0)||(query2 != 0)){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    DateCheck(posted_date);
}

int Description(int cond){
    float buyitnow;
    int query;
    int query1;
    char desc[200];
    char Descquery[200];
    char Pricequery[200];

    printf("---- description: ");
    getchar();
    scanf(" %[^\n]s", desc);
    

    printf("---- buy-it-now price: ");
    scanf("%f", &buyitnow);
    float namu = buyitnow - (int)buyitnow;
    int a = 0;
    int Offerprice;

    while (a == 0) {
        if (namu != 0){
            printf("Your input is not numeric, please check it again.");
            printf("---- buy-it-now price: ");
            scanf("%f", &buyitnow);
            float namu = buyitnow - (int)buyitnow;
        }
        else{
            Offerprice = (int)buyitnow;
            a = 1;
        }
    }
    sprintf(Descquery, "UPDATE item SET item_description = '%s' where item_id = '%d';", desc, cond);
    sprintf(Pricequery, "UPDATE item SET price = %d where item_id = '%d';", Offerprice, cond);
    query = mysql_query(conn, Descquery);
    query1 = mysql_query(conn, Pricequery);
    if ((query != 0) || (query1 != 0)){
        fprintf(stderr, "Mysql query error10 : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    Bid_Ending(cond);
}

int Condition(int cond){
    int query;
    int query2;
    int d;
    char Conditionquery[200];
    char Condquery[200];

    printf("\n---- condition: (Enter the number)\n(1) New\n(2) Like-New\n(3) Used (Good)\n(4) Used (Acceptable)\n");
    scanf("%d", &d);
    switch(d){
        case 1:
        sprintf(Conditionquery, "insert into item_info(item_id) values ('%d');", cond);
        sprintf(Condquery, "UPDATE item_info SET cond = 'New' where item_id = '%d';", cond);
        query = mysql_query(conn, Conditionquery);
        query2 = mysql_query(conn, Condquery);
        if ((query != 0) || (query2 != 0)){
            fprintf(stderr, "Mysql query error6 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        Description(cond);
        break;
        case 2:
        sprintf(Conditionquery, "insert into item_info(item_id) values ('%d');", cond);
        sprintf(Condquery, "UPDATE item_info SET cond = 'Like-New' where item_id = '%d';", cond);
        query = mysql_query(conn, Conditionquery);
        query2 = mysql_query(conn, Condquery);
        if ((query != 0) || (query2 != 0)){
            fprintf(stderr, "Mysql query error7 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        Description(cond);
        break;
        case 3:
        sprintf(Conditionquery, "insert into item_info(item_id) values ('%d');", cond);
        sprintf(Condquery, "UPDATE item_info SET cond = 'Used (Good)' where item_id = '%d';", cond);
        query = mysql_query(conn, Conditionquery);
        query2 = mysql_query(conn, Condquery);
        if ((query != 0) || (query2 != 0)){
            fprintf(stderr, "Mysql query error8 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        Description(cond);
        break;
        case 4:
        sprintf(Conditionquery, "insert into item_info(item_id) values ('%d');", cond);
        sprintf(Condquery, "UPDATE item_info SET cond = 'Used (Acceptable)' where item_id = '%d';", cond);
        query = mysql_query(conn, Conditionquery);
        query2 = mysql_query(conn, Condquery);
        if ((query != 0) || (query2 != 0)){
            fprintf(stderr, "Mysql query error9 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        Description(cond);
        break;
    }
}

int SellItem(char *email){
    int query;
    int query2;
    int query3;
    int c;
    int cond;
    char SellItemquery[200];
    char Catequery[200];
    char item_query[200];

    printf("\n----< Sell item >\n---- select from the following category : (Enter the number)\n(1) Electronics\n(2) Books\n(3) Home\n(4) Clothing\n(5) Sporting Goods\n");
    scanf("%d", &c);

    switch(c){
        case 1:
        sprintf(SellItemquery, "insert into item(seller) values ('%s');", email);
        sprintf(Catequery, "UPDATE item SET category = 'Electronics' where seller = '%s' and category is NULL;", email);
        sprintf(item_query, "select item_id from item where category = 'Electronics' and seller = '%s' order by item_id desc limit 1;", email);
        query = mysql_query(conn, SellItemquery);
        query2 = mysql_query(conn, Catequery);
        query3 = mysql_query(conn, item_query);
        if ((query != 0) || (query2 != 0) || (query3 != 0)){
            fprintf(stderr, "Mysql query error1 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        cond = atoi(row[0]);
        mysql_free_result(result);
        Condition(cond);
        break;
        case 2:
        sprintf(SellItemquery, "insert into item(seller) values ('%s');", email);
        sprintf(Catequery, "UPDATE item SET category = 'Books' where seller = '%s' and category is NULL;", email);
        sprintf(item_query, "select item_id from item where category = 'Books' and seller = '%s' order by item_id desc limit 1;", email);
        query = mysql_query(conn, SellItemquery);
        query2 = mysql_query(conn, Catequery);
        query3 = mysql_query(conn, item_query);
        if ((query != 0) || (query2 != 0) || (query3 != 0)){
            fprintf(stderr, "Mysql query error2 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        cond = atoi(row[0]);
        mysql_free_result(result);
        Condition(cond);
        break;
        case 3:
        sprintf(SellItemquery, "insert into item(seller) values ('%s');", email);
        sprintf(Catequery, "UPDATE item SET category = 'Home' where seller = '%s' and category is NULL;", email);
        sprintf(item_query, "select item_id from item where category = 'Home' and seller = '%s' order by item_id desc limit 1;", email);
        query = mysql_query(conn, SellItemquery);
        query2 = mysql_query(conn, Catequery);
        query3 = mysql_query(conn, item_query);
        if ((query != 0) || (query2 != 0) || (query3 != 0)){
            fprintf(stderr, "Mysql query error3 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        cond = atoi(row[0]);
        mysql_free_result(result);
        Condition(cond);
        break;
        case 4:
        sprintf(SellItemquery, "insert into item(seller) values ('%s');", email);
        sprintf(Catequery, "UPDATE item SET category = 'Clothing' where seller = '%s' and category is NULL;", email);
        sprintf(item_query, "select item_id from item where category = 'Clothing' and seller = '%s' order by item_id desc limit 1;", email);
        query = mysql_query(conn, SellItemquery);
        query2 = mysql_query(conn, Catequery);
        query3 = mysql_query(conn, item_query);
        if ((query != 0) || (query2 != 0) || (query3 != 0)){
            fprintf(stderr, "Mysql query error4 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        cond = atoi(row[0]);
        mysql_free_result(result);
        Condition(cond);
        break;
        case 5:
        sprintf(SellItemquery, "insert into item(seller) values ('%s');", email);
        sprintf(Catequery, "UPDATE item SET category = 'Sporting Goods' where seller = '%s' and category is NULL;", email);
        sprintf(item_query, "select item_id from item where category = 'Sporting Goods' and seller = '%s' order by item_id desc limit 1;", email);
        query = mysql_query(conn, SellItemquery);
        query2 = mysql_query(conn, Catequery);
        query3 = mysql_query(conn, item_query);
        if ((query != 0) || (query2 != 0) || (query3 != 0)){
            fprintf(stderr, "Mysql query error5 : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        cond = atoi(row[0]);
        mysql_free_result(result);
        Condition(cond);
        break;
    }
}

int SellerMenu(char *email){
    int count;
    int query;
    int query2;
    int query3;
    int query4;
    int query5;
    int query6;
    char Item_infoquery[300];
    char Desc_infoquery[300];
    char Countquery[300];
    char Statusquery[300];
    char Sold_infoquery[300];
    char status[6];

    printf("----< Status of Your Item Listed on Auction >\n");
    sprintf(Item_infoquery, "select item_id from item where seller = '%s';", email); 
    query2 = mysql_query(conn, Item_infoquery);
    if (query2 != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    int i = 0;
    int j = 0;
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query5 = mysql_query(conn, Statusquery);
        if (query5 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);
        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d' and offer_price != 0;", count);
            query3 = mysql_query(conn, Countquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result1 = mysql_store_result(conn);
            row = mysql_fetch_row(result1);
            int num = atoi(row[0]);
            mysql_free_result(result1);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query4 = mysql_query(conn, Desc_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result2);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                mysql_free_result(result2);
                i += 1;
                j = i;
            }
        }

        else{
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query6 = mysql_query(conn, Sold_infoquery);
            if (query6 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                mysql_free_result(result3);
                i += 1;
                j = i;
            }
        }
    }
    mysql_free_result(result);
    exit(0);
}

int Category(char *email){
    char c;
    int f;
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int count;
    int price;
    char p[10];
    char cate[30];
    char Item_infoquery[300];
    char Buyquery[300];
    char Bidquery[300];
    char Bid1query[300];
    char Tradequery[300];
    char Trade1query[300];
    char Infoquery[300];
    char Desc_infoquery[300];
    char Countquery[300];
    char Statusquery[300];
    char Sold_infoquery[300];
    char status[6];
    char seller[20];
    char findquery[75];

    printf("\n----< Search items by category > : (Enter the number)\n(1) Electronics\n(2) Books\n(3) Home\n(4) Clothing\n(5) Sporting Goods\n");
    scanf("%d", &f);
    switch(f){
        case 1:
        sprintf(cate, "Electronics");
        break;
        case 2:
        sprintf(cate, "Books");
        break;
        case 3:
        sprintf(cate, "Home");
        break;
        case 4:
        sprintf(cate, "Clothing");
        break;
        case 5:
        sprintf(cate, "Sporting");
    }

    int i = 0;
    int j = 0;
    char purchase_date[20];
    struct tm *t;
    time_t timer;

    timer = time(NULL);    // 현재 시각을 초 단위로 얻기
    t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
    sprintf(purchase_date, "%04d-%02d-%02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    DateCheck(purchase_date);

    sprintf(Item_infoquery, "select item_id, seller, price from item where category = '%s';", cate); 
    query = mysql_query(conn, Item_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);

    printf("\n----< Search results: Category>\n");
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(seller, "%s", row[1]);
        price = atoi(row[2]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query1 = mysql_query(conn, Statusquery);
        if (query1 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);
        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d';", count);
            query2 = mysql_query(conn, Countquery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            row = mysql_fetch_row(result2);
            int num = atoi(row[0]);
            mysql_free_result(result2);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query3 = mysql_query(conn, Desc_infoquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);

            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num-1);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                i += 1;
                j = i;
            }
            mysql_free_result(result3);
        }
        else {
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query4 = mysql_query(conn, Sold_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result4 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result4);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                i += 1;
                j = i;
            }
            mysql_free_result(result4);
        }
    }
    mysql_free_result(result);

    printf("\n--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu): ");
    getchar();
    scanf("%c", &c);
    int a = c - '0' - 1;
    if (c == 'B'){
        Category(email);
    }
    else{
        sprintf(findquery, "select item_id from item where category = '%s' limit %d, 1;", cate, a);
        query3 = mysql_query(conn, findquery);
        if (query3 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result2 = mysql_store_result(conn);
        row = mysql_fetch_row(result2);
        int this = atoi(row[0]);
        mysql_free_result(result2);

        printf("\n--- Bidding price? ");
        scanf("%s", p);
        if ( strcmp(p, "buy") == 0){
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, price);
            sprintf(Bid1query, "insert into trade values ('%s', '%s', '%d', %d, '%s');", email, seller, this, price, purchase_date);
            sprintf(Infoquery, "UPDATE item_info SET pur_status = 'Sold' where item_id = %d;", this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Bid1query);
            query2 = mysql_query(conn, Infoquery);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
        else{
            int pri = atoi(p);
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, pri);
            sprintf(Tradequery, "insert ignore into trade value('%s', '%s', '%d', '%d', '%s');", email, seller, this, pri, purchase_date);
            sprintf(Trade1query, "UPDATE trade SET final_price = (select max(offer_price) from bid_history where item_id = '%d') where item_id = '%d';", this, this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Tradequery);
            query2 = mysql_query(conn, Trade1query);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            } 
        }
    }   
}

int Keyword(char *email){
    char c;
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int count;
    int price;
    char p[10];
    char keyword[30];
    char Item_infoquery[300];
    char Buyquery[300];
    char Bidquery[300];
    char Bid1query[300];
    char Tradequery[300];
    char Trade1query[300];
    char Infoquery[300];
    char Desc_infoquery[300];
    char Countquery[300];
    char Statusquery[300];
    char Sold_infoquery[300];
    char status[6];
    char seller[20];
    char findquery[75];
    
    printf("\n----< Search items by description keyword >\n---- Search keyword : (Enter the keyword)\n");
    scanf("%s", keyword);

    int i = 0;
    int j = 0;
    char purchase_date[20];
    struct tm *t;
    time_t timer;

    timer = time(NULL);    // 현재 시각을 초 단위로 얻기
    t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
    sprintf(purchase_date, "%04d-%02d-%02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

    DateCheck(purchase_date);

    sprintf(Item_infoquery, "select item_id, seller, price from item where item_description like '%%%s%%';", keyword); 
    query = mysql_query(conn, Item_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);

    printf("\n----< Search results: Keyword>\n");
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(seller, "%s", row[1]);
        price = atoi(row[2]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query1 = mysql_query(conn, Statusquery);
        if (query1 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);
        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d' and offer_price != 0;", count);
            query2 = mysql_query(conn, Countquery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            row = mysql_fetch_row(result2);
            int num = atoi(row[0]);
            mysql_free_result(result2);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query3 = mysql_query(conn, Desc_infoquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);
            
            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                i += 1;
                j = i;
            }
            mysql_free_result(result3);
        }
        else {
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query4 = mysql_query(conn, Sold_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result4 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result4);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                i += 1;
                j = i;
            }
            mysql_free_result(result4); 
        }
    }
    mysql_free_result(result);

    printf("\n--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu): ");
    getchar();
    scanf("%c", &c);
    int a = c - '0' - 1;
    if (c == 'B'){
        Keyword(email);
    }
    else{
        sprintf(findquery, "select item_id from item where item_description like '%%%s%%' limit %d, 1;", keyword, a);
        query3 = mysql_query(conn, findquery);
        if (query3 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result6 = mysql_store_result(conn);
        row = mysql_fetch_row(result6);
        int this = atoi(row[0]);
        mysql_free_result(result6);

        printf("\n--- Bidding price? ");
        scanf("%s", p);
        if ( strcmp(p, "buy") == 0){
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, price);
            sprintf(Bid1query, "insert into trade values ('%s', '%s', '%d', %d, '%s');", email, seller, this, price, purchase_date);
            sprintf(Infoquery, "UPDATE item_info SET pur_status = 'Sold' where item_id = %d;", this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Bid1query);
            query2 = mysql_query(conn, Infoquery);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
        else{
            int pri = atoi(p);
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, pri);
            sprintf(Tradequery, "insert ignore into trade value('%s', '%s', '%d', '%d', '%s');", email, seller, this, pri, purchase_date);
            sprintf(Trade1query, "UPDATE trade SET final_price = (select max(offer_price) from bid_history where item_id = '%d') where item_id = '%d';", this, this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Tradequery);
            query2 = mysql_query(conn, Trade1query);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
    }
}

int Seller(char *email){
    char c;
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int count;
    int price;
    char p[10];
    char seller[30];
    char Item_infoquery[300];
    char Buyquery[300];
    char Bidquery[300];
    char Bid1query[300];
    char Tradequery[300];
    char Trade1query[300];
    char Infoquery[300];
    char Desc_infoquery[300];
    char Countquery[300];
    char Statusquery[300];
    char Sold_infoquery[300];
    char status[6];
    char findquery[75];

    printf("\n----< Search items by seller >\n---- seller : ");
    scanf("%s", seller);
    
    int i = 0;
    int j = 0;
    char purchase_date[20];
    struct tm *t;
    time_t timer;

    timer = time(NULL);    // 현재 시각을 초 단위로 얻기
    t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
    sprintf(purchase_date, "%04d-%02d-%02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

    DateCheck(purchase_date);

    sprintf(Item_infoquery, "select item_id, price from item where seller = '%s';", seller); 
    query = mysql_query(conn, Item_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);

    printf("\n----< Search results: Seller>\n");
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        price = atoi(row[1]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query1 = mysql_query(conn, Statusquery);
        if (query1 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);
        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d';", count);
            query2 = mysql_query(conn, Countquery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            row = mysql_fetch_row(result2);
            int num = atoi(row[0]);
            mysql_free_result(result2);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query3 = mysql_query(conn, Desc_infoquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);
            
            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num-1);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                i += 1;
                j = i;
            }
            mysql_free_result(result3);
        }
        else {
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query4 = mysql_query(conn, Sold_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result4 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result4);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                i += 1;
                j = i;
            }
            mysql_free_result(result4);    
        }
    }
    mysql_free_result(result);

    printf("\n--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu): ");
    getchar();
    scanf("%c", &c);
    int a = c - '0' - 1;
    if (c == 'B'){
        Seller(email);
    }
    else{
        sprintf(findquery, "select item_id from item where seller = '%s' limit %d, 1;", seller, a);
        query3 = mysql_query(conn, findquery);
        if (query3 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result2 = mysql_store_result(conn);
        row = mysql_fetch_row(result2);
        int this = atoi(row[0]);
        mysql_free_result(result2);

        printf("\n--- Bidding price? ");
        scanf("%s", p);
        if ( strcmp(p, "buy") == 0){
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, price);
            sprintf(Bid1query, "insert into trade values ('%s', '%s', '%d', %d, '%s');", email, seller, this, price, purchase_date);
            sprintf(Infoquery, "UPDATE item_info SET pur_status = 'Sold' where item_id = %d;", this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Bid1query);
            query2 = mysql_query(conn, Infoquery);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
        else{
            int pri = atoi(p);
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, pri);
            sprintf(Tradequery, "insert ignore into trade value('%s', '%s', '%d', '%d', '%s');", email, seller, this, pri, purchase_date);
            sprintf(Trade1query, "UPDATE trade SET final_price = (select max(offer_price) from bid_history where item_id = '%d') where item_id = '%d';", this, this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Tradequery);
            query2 = mysql_query(conn, Trade1query);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
    }
}

int DatePosted(char *email){
    char c;
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int count;
    int price;
    char p[10];
    char date[30];
    char Item_infoquery[300];
    char Buyquery[300];
    char Bidquery[300];
    char Bid1query[300];
    char Tradequery[300];
    char Trade1query[300];
    char Infoquery[300];
    char Desc_infoquery[300];
    char Countquery[300];
    char Statusquery[300];
    char Sold_infoquery[300];
    char status[6];
    char seller[20];
    char findquery[75];

    printf("\n----< Search items by date >\n---- date posted : ");
    scanf("%s", date);

    int i = 0;
    int j = 0;
    char purchase_date[20];
    struct tm *t;
    time_t timer;

    timer = time(NULL);
    t = localtime(&timer);
    sprintf(purchase_date, "%04d-%02d-%02d %02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

    DateCheck(purchase_date);

    sprintf(Item_infoquery, "select item_id, seller, price from item natural join item_info where posted_date like '%%%s%%';", date); 
    query = mysql_query(conn, Item_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    
    printf("\n----< Search results: Posted Date >\n");
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(seller, "%s", row[1]);
        price = atoi(row[2]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query1 = mysql_query(conn, Statusquery);
        if (query1 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);

        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d';", count);
            query2 = mysql_query(conn, Countquery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            row = mysql_fetch_row(result2);
            int num = atoi(row[0]);
            mysql_free_result(result2);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query3 = mysql_query(conn, Desc_infoquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);

            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                i += 1;
                j = i;
            }
            mysql_free_result(result3);
        }
        else {
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query4 = mysql_query(conn, Sold_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result4 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result4);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                i += 1;
                j = i;
            }
            mysql_free_result(result4);
        }
    }
    mysql_free_result(result);
    
    printf("\n--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu): ");
    getchar();
    scanf("%c", &c);
    int a = c - '0' - 1;
    if (c == 'B'){
        DatePosted(email);
    }
    else{
        sprintf(findquery, "select item_id from item_info where posted_date like '%%%s%%' limit %d, 1;", date, a);
        query3 = mysql_query(conn, findquery);
        if (query3 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result2 = mysql_store_result(conn);
        row = mysql_fetch_row(result2);
        int this = atoi(row[0]);
        mysql_free_result(result2);

        printf("\n--- Bidding price? ");
        scanf("%s", p);
        if ( strcmp(p, "buy") == 0){
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, price);
            sprintf(Bid1query, "insert into trade values ('%s', '%s', '%d', %d, '%s');", email, seller, this, price, purchase_date);
            sprintf(Infoquery, "UPDATE item_info SET pur_status = 'Sold' where item_id = %d;", this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Bid1query);
            query2 = mysql_query(conn, Infoquery);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
        else{
            int pri = atoi(p);
            sprintf(Bidquery, "insert into bid_history values ('%d', '%s', '%s', %d);", this, email, purchase_date, pri);
            sprintf(Tradequery, "insert ignore into trade value('%s', '%s', '%d', '%d', '%s');", email, seller, this, pri, purchase_date);
            sprintf(Trade1query, "UPDATE trade SET final_price = (select max(offer_price) from bid_history where item_id = '%d') where item_id = '%d';", this, this);
            query = mysql_query(conn, Bidquery);
            query1 = mysql_query(conn, Tradequery);
            query2 = mysql_query(conn, Trade1query);
            if ((query != 0) || (query1 != 0) || (query2 != 0)){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
        }
    }
}

int SearchItem(char *email){
    int e;
    printf("\n----< Search item > : (Enter the number)\n----(1) Search items by category\n----(2) Search items by description keyword\n----(3) Search items by seller\n----(4) Search items by date posted\n----(5) Go Back\n----(6) Quit\n");
    scanf("%d", &e);
    switch(e){
        case 1:
        Category(email);
        break;
        case 2:
        Keyword(email);
        break;
        case 3:
        Seller(email);
        break;
        case 4:
        DatePosted(email);
        break;
        case 5:
        break;
        case 6:
        printf("Bye\n");
        exit(0);
    }
}

int BuyerMenu(char *email){
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int count;
    char status[6];
    char Item_infoquery[300];
    char Statusquery[300];
    char Countquery[300];
    char Desc_infoquery[300];
    char Sold_infoquery[300];
    
    printf("\n----< Check Status of your Bid >\n");
    
    sprintf(Item_infoquery, "select distinct item_id from item natural join bid_history where buyer = '%s';", email); 
    query = mysql_query(conn, Item_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    int i = 0;
    int j = 0;
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(Statusquery, "select pur_status from item_info where item_id = '%d';", count);
        query1 = mysql_query(conn, Statusquery);
        if (query1 != 0){
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        MYSQL_RES *result5 = mysql_store_result(conn);
        row = mysql_fetch_row(result5);
        sprintf(status, "%s", row[0]);
        mysql_free_result(result5);

        if(strcmp("Sold", status) != 0){
            sprintf(Countquery, "select count(item_id) from bid_history where item_id = '%d';", count);
            query2 = mysql_query(conn, Countquery);
            if (query2 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result2 = mysql_store_result(conn);
            row = mysql_fetch_row(result2);
            int num = atoi(row[0]);
            mysql_free_result(result2);
            sprintf(Desc_infoquery, "select item_description, offer_price, buyer, posted_date, bid_ending_date from (item natural join item_info) natural join bid_history where item_id = '%d' order by offer_price desc limit 1;", count);
            query3 = mysql_query(conn, Desc_infoquery);
            if (query3 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result3 = mysql_store_result(conn);

            for (int i = j; row = mysql_fetch_row(result3);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: %d bids\n", num-1);
                printf("current bidding price: %s\n", row[1]);
                printf("current highest bidder: %s\n", row[2]);
                printf("date posted: %s\n", row[3]);
                printf("bid ending date: %s\n", row[4]);
                i += 1;
                j = i;
            }
            mysql_free_result(result3);
        }
        else {
            sprintf(Sold_infoquery, "select item_description, final_price, buyer, purchase_date from item natural join trade where item_id = '%d';", count);
            query4 = mysql_query(conn, Sold_infoquery);
            if (query4 != 0){
                fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
                mysql_close(conn);
                return 1;
            }
            MYSQL_RES *result4 = mysql_store_result(conn);
            for (int i = j; row = mysql_fetch_row(result4);){
                printf("[Item %d]\n", i+1);
                printf("decription: %s\n", row[0]);
                printf("status: sold\n");
                printf("sold price: %s\n", row[1]);
                printf("buyer: %s\n", row[2]);
                printf("sold date: %s\n", row[3]);
                i += 1;
                j = i;
            }
            mysql_free_result(result4);
        }
    }
    mysql_free_result(result);
}

int Account(char *email){
    int query;
    int query1;
    int query2;
    char Sold_numquery[100];
    char Purch_numquery[100];
    char Sold_infoquery[300];
    char Purch_infoquery[300];
    char buysum[70];
    char sellsum[100];
    char partsum[110];
    
    printf("\n----< Check your Account >\n");
    sprintf(Sold_infoquery, "select item_description, final_price from trade natural join item where seller = '%s';", email); 
    query = mysql_query(conn, Sold_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);

    for (int i = 0; row = mysql_fetch_row(result); i++){
        printf("[Sold Item %d]\n", i+1);
        printf("decription: %s\n",row[0]);
        printf("sold price: %s\n",row[1]);
    }
    mysql_free_result(result);

    sprintf(Purch_infoquery, "select item_description, final_price from trade natural join item where buyer = '%s';", email); 
    query = mysql_query(conn, Purch_infoquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    printf("\n");
    for (int i = 0; row = mysql_fetch_row(result); i++){
        printf("[Purchased Item %d]\n", i+1);
        printf("decription: %s\n",row[0]);
        printf("sold price: %s\n",row[1]);
    }
    mysql_free_result(result);
    printf("\n");
    
    sprintf(buysum, "select sum(final_price) from trade where buyer = '%s';", email);
    query1 = mysql_query(conn, buysum);
    if (query1 != 0) {
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    int buy = atoi(row[0]);
    mysql_free_result(result);

    sprintf(sellsum, "select count(seller), sum(final_price) from trade where seller = '%s' and final_price!=0;", email); 
    query2 = mysql_query(conn, sellsum);
    if (query2 != 0) {
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    int sellcount = atoi(row[0]);
    int sell = atoi(row[1]);
    printf("sellcount: %d, sell: %d\n", sellcount, sell);
    mysql_free_result(result);
    if (sellcount > 2){
        sprintf(partsum, "select final_price from trade where seller = '%s' and final_price!=0 order by purchase_date asc limit 2;", email); 
        query2 = mysql_query(conn, partsum);
        if (query2 != 0) {
            fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
            mysql_close(conn);
            return 1;
        }
        int sum = 0;
        result = mysql_store_result(conn);
        while (row = mysql_fetch_row(result)){
            sum += atoi(row[0]);
        }
        printf("sum : %d\n", sum);
        mysql_free_result(result);
        double comm = (double)(sum*0.02 + (sell-sum)*0.01);
        printf("[Your Balance Summary]\n");
        printf("sold: %d won\n", sell);
        printf("commision: %f won\n", -comm);
        printf("purchased: %d won\n", -buy);
        printf("Total Balance: %f won\n",sell - comm - buy);
    }
    else{
        printf("[Your Balance Summary]\n");
        printf("sold: %d won\n", sell);
        printf("commision: %f won\n", -(double)sell*0.02);
        printf("purchased: %d won\n", -buy);
        printf("Total Balance: %f won\n", (double)sell*0.98 - buy);
    }
}

int MainMenu(char *email){
    int b;
    MainM:
        printf("\n----< Main menu > : (Enter the number)\n----(1) Sell item\n----(2) Status of Your Item Listed on Auction\n----(3) Search item\n----(4) Check Status of your Bid\n----(5) Check your Account\n----(Q) Quit\n");
        scanf("%d", &b);
        switch(b){
            case 1:
            SellItem(email);
            MainMenu(email);
            case 2:
            SellerMenu(email);
            break;
            case 3:
            SearchItem(email);
            MainMenu(email);
            break;
            case 4:
            BuyerMenu(email);
            break;
            case 5:
            Account(email);
            break;
            case 'Q':
            printf("Bye\n");
            exit(0);
        }
}

int Login(){
    int query;
    char email[50];
    char pw[20];
    char Loginquery[200];
    printf("----< Login >\n---- email: ");
    scanf("%s", email);
    printf("---- password: ");
    scanf("%s", pw);

    sprintf(Loginquery, "select pw from user where Admin = 0 and email = '%s';", email);
    query = mysql_query(conn, Loginquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    result = mysql_store_result(conn);
    if(strcmp(pw, mysql_fetch_row(result)[0]) != 0){
        printf("mysql_store_result: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    mysql_free_result(result);
    MainMenu(email);
}

int SignUp(){
    int query;
    char firn[30];
    char secn[15];
    char email[50];
    char pw[20];
    char SignUpquery[200];
    printf("----< Sign up >\n---- first name: ");
    scanf("%s", firn);
    printf("---- last name: ");
    scanf("%s", secn);
    printf("---- email: ");
    scanf("%s", email);
    printf("---- password: ");
    scanf("%s", pw);
    sprintf(SignUpquery, "insert into user values ('%s', '%s', '%s', '%s', '0');", email, firn, secn, pw);
    query = mysql_query(conn, SignUpquery);
    if (query != 0){
      fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
      mysql_close(conn);
      return 1;
    }
    return 1;
    //error handling -> 이미 있는 이메일 / 길이 초과 / 데베 insert
}

int Memember_Info(){
    int query;
    char Memberquery[100];

    sprintf(Memberquery, "select email, first_name, last_name, pw from user where Admin = 0;");
    query = mysql_query(conn, Memberquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    for (int i = 0; row = mysql_fetch_row(result); i++){
        printf("ID: %s, ", row[0]);
        printf("First name: %s, ",row[1]);
        printf("Last name: %s, ",row[2]);
        printf("pw: %s",row[3]);
        printf("\n");
    }
    mysql_free_result(result);
}

int NumPro(){
    int query;
    char NumProquery[100];

    sprintf(NumProquery, "select category, count(item_id) as number_of_product from item group by category;");
    query = mysql_query(conn, NumProquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    for (int i = 0; row = mysql_fetch_row(result); i++){
        printf("%s(%s)\n", row[0], row[1]);
    }
    mysql_free_result(result);
}

int Delete(){
    int count;
    int query;
    int query1;
    int query2;
    int query3;
    int query4;
    int query5;
    char Deletequery[100];
    char Delete1query[100];
    char Delete2query[100];
    char Delete3query[100];
    char Delete4query[100];
    char email[30];
    char id_infoquery[70];

    printf("Which user do you want to delete? Enter the user id: ");
    scanf("%s", email);
    printf("All usage datails of %s have been deleted\n", email);
    sprintf(Deletequery, "DELETE FROM bid_history where buyer = '%s';", email);
    sprintf(Delete1query, "DELETE FROM trade where seller = '%s' or buyer = '%s';", email, email);
    sprintf(Delete2query, "DELETE FROM user where email = '%s';", email);
    query = mysql_query(conn, Deletequery);
    query1 = mysql_query(conn, Delete1query);
    query2 = mysql_query(conn, Delete2query);
    if ((query != 0) || (query1 != 0) || (query2 != 0)){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    sprintf(id_infoquery, "select item_id from item where seller = '%s';", email); 
    query3 = mysql_query(conn, id_infoquery);
    if (query3 != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    while (row = mysql_fetch_row(result)){
        count = atoi(row[0]);
        sprintf(Delete3query, "DELETE FROM item_info where item_id = '%d';", count);
        sprintf(Delete4query, "DELETE FROM item where item_id = '%d';", count);
        query4 = mysql_query(conn, Delete3query);
        query5 = mysql_query(conn, Delete3query);
        if ((query4 != 0) || (query5 != 0)){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
        }
    }
}

int MenuforAdmin(){
    int a;
    printf("\n----< Administrator >\n----(1) Member Information\n----(2) Number of Products by category\n----(3) Delete a User\n");
        scanf("%d", &a);
        switch(a){
            case 1:
            Memember_Info();
            break;
            case 2:
            NumPro();
            break;
            case 3:
            Delete();
            DEFAULT:
            exit(0);
        }
}

int LoginAdmin(){
    int query;
    char email[50];
    char pw[20];
    char Loginquery[200];
    printf("----< Login as Administrator >\n---- email: ");
    scanf("%s", email);
    printf("---- password: ");
    scanf("%s", pw); 
    sprintf(Loginquery, "select pw from user where email = '%s' and admin = 1;", email);
    query = mysql_query(conn, Loginquery);
    if (query != 0){
        fprintf(stderr, "Mysql query error : %s", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }
    result = mysql_store_result(conn);
    if(strcmp(pw, mysql_fetch_row(result)[0]) != 0){
        printf("mysql_store_result: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    mysql_free_result(result);
    MenuforAdmin();
}

int LoginMenu(){
    int a;
    SignUpLogin:
        printf("\n----< Login menu >\n----(1) Login\n----(2) Sign up\n----(3) Login as Administrator\n----(4) Quit\n");
        scanf("%d", &a);
        switch(a){
            case 1:
            Login();
            break;
            case 2:
            SignUp();
            goto SignUpLogin;
            break;
            case 3:
            LoginAdmin();
            break;
            case 4:
            printf("Bye\n");
            exit(0);
        }
        //a가 1~4 아니면 다시
}

int main(int argc, char *argv[]) {
    
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "localhost", "localhost", "password", "localhost", portnum, NULL, 0)){
        printf("mysql_real_connect: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }

    char *sqlquery1 = "CREATE TABLE IF NOT EXISTS user (email varchar(50), first_name varchar(30), last_name varchar(15), pw varchar(20) NOT NULL, admin numeric(1) DEFAULT 0, primary key (email));";
    char *sqlquery2 = "CREATE TABLE IF NOT EXISTS item (seller varchar(45), item_id int(10) NOT NULL AUTO_INCREMENT , category varchar(15), price numeric(6) check (price>=0), item_description varchar(100), primary key (item_id), foreign key (seller) references user (email) on delete set null);";
    char *sqlquery3 = "CREATE TABLE IF NOT EXISTS item_info (item_id int(10), cond varchar(20), posted_date varchar(20), bid_ending_date varchar(20), pur_status varchar(5) DEFAULT 'Valid', primary key (item_id), foreign key (item_id) references item (item_id) ON UPDATE CASCADE);";
    char *sqlquery4 = "CREATE TABLE IF NOT EXISTS trade (buyer varchar(45), seller varchar(45), item_id int(10), final_price numeric(6) check (final_price>=0), purchase_date varchar(20), primary key (item_id), foreign key (item_id) references item (item_id), foreign key (buyer) references user (email), foreign key (seller) references user (email));";
    char *sqlquery5 = "CREATE TABLE IF NOT EXISTS bid_history (item_id int(10), buyer varchar(50) DEFAULT NULL, bid_date varchar(20), offer_price numeric(6), primary key (item_id, bid_date), foreign key (item_id) references item(item_id), foreign key (buyer) references user(email));";
    char *sqlquery6 = "insert ignore into user value ('Admin', 'HanWool', 'Huh', '1234', 1);";
    if (mysql_query(conn, sqlquery1) || mysql_query(conn, sqlquery2) || mysql_query(conn, sqlquery3) || mysql_query(conn, sqlquery4) || mysql_query(conn, sqlquery5) || mysql_query(conn, sqlquery6)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    
    LoginMenu();
}
