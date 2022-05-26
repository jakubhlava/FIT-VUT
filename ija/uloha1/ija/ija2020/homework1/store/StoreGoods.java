package ija.ija2020.homework1.store;

import java.util.ArrayList;

import java.time.LocalDate;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;
import ija.ija2020.homework1.store.StoreGoodsItem;

public class StoreGoods implements Goods {

    private String name;
    private ArrayList<GoodsItem> items;

    public StoreGoods(String name) {
        this.name = name;
        this.items = new ArrayList<GoodsItem>();
    }

    public String getName() {
        return this.name;
    }

    public boolean addItem(GoodsItem var1) {
        return this.items.add(var1);
    }

    public boolean remove(GoodsItem var1) {
        return this.items.remove(var1);
    }

    public boolean empty() {
        if (this.items.size() == 0) {
            return true;
        } else {
            return false;
        }
    }

    public GoodsItem newItem(LocalDate loaded) {
        GoodsItem item = new StoreGoodsItem(this, loaded);
        this.addItem(item);
        return item;
    }

    public int size() {
        return this.items.size();
    }

    public boolean equals(Object obj) {
        if (!(obj instanceof Goods)) {
            return false;
        }
        Goods goods = (Goods) obj;
        if (goods.getName() == this.name) {
            return true;
        } else {
            return false;
        }
    }

    public int hashCode() {
        return 1;
    }

}
