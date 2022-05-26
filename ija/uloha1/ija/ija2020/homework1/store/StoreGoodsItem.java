package ija.ija2020.homework1.store;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;

import java.time.LocalDate;

public class StoreGoodsItem implements GoodsItem {

    Goods goods;
    LocalDate date;
    public StoreGoodsItem(Goods goods, LocalDate date) {
        this.goods = goods;
        this.date = date;
    }

    public Goods goods() {
        return this.goods;
    }

    public boolean sell() {
        return this.goods.remove(this);
    }
}
