create table user
    (email varchar(50),
     first_name varchar(30),
     last_name varchar(15),
     pw varchar(20) NOT NULL,
     admin numeric(1) DEFAULT 0,
     primary key (email)
    );

create table item
    (seller varchar(45),
     item_id int(10) NOT NULL AUTO_INCREMENT,
     category varchar(15),
     price numeric(6) check (price>=0),
     item_description varchar(100),
     primary key (item_id),
     foreign key (seller) references user (email)
        on delete set null
    );

create table item_info
    (item_id int(10),
     cond varchar(20),
     posted_date varchar(20),
     bid_ending_date varchar(20),
     pur_status varchar(5) DEFAULT 'Valid',
     primary key (item_id),
     foreign key (item_id) references item (item_id)
        ON UPDATE CASCADE
    );

create table trade
    (buyer varchar(45),
     seller varchar(45),
     item_id int(10),
     final_price numeric(6) check (final_price>=0),
     purchase_date varchar(20),
     primary key (item_id),
     foreign key (item_id) references item (item_id),
     foreign key (buyer) references user (email),
     foreign key (seller) references user (email)
    );

create table bid_history
    (item_id int(10),
     buyer varchar(50) DEFAULT NULL,
     bid_date varchar(20),
     offer_price numeric(6),
     primary key (item_id, bid_date),
     foreign key (item_id) references item(item_id),
     foreign key (buyer) references user(email)
    );