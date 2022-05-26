package ija.ija2020.homework1.store;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;
import ija.ija2020.homework1.goods.GoodsShelf;

import java.util.ArrayList;
import java.util.List;

public class StoreShelf implements GoodsShelf {

    private List<GoodsItem> shelf;

    public StoreShelf() {
        this.shelf = new ArrayList<>();
    }

    public void put(GoodsItem item) {
        shelf.add(item);
    }

    public GoodsItem removeAny(Goods goods) {
        for (GoodsItem item:
             shelf) {
            if (item.goods().equals(goods)) {
                shelf.remove(item);
                return item;
            }
        }
        return null;
    }

    public boolean containsGoods(Goods goods) {
        for (GoodsItem item: shelf) {
            if (item.goods().equals(goods)) {
                return true;
            }
        }
        return false;
    }

    public int size(Goods goods) {
        int counter = 0;
        for (GoodsItem item: shelf) {
            if (item.goods().equals(goods)) {
                counter++;
            }
        }
        return counter;
    }
}
