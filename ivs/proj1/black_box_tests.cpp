//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Jakub Hlava <xhlava52@stud.fit.vutbr.cz>
// $Date:       $2020-02-23
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Jakub Hlava
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class EmptyTree : public ::testing::Test {
protected:
    BinaryTree tree;
};

class NonEmptyTree : public ::testing::Test {
protected:
    void SetUp() override {
        std::vector<int> nodes = { 10,1,15,2,3,89,85,90,4,7,9 };
        std::vector<std::pair<bool, Node_t*>> output;
        tree.InsertNodes(nodes, output);
    }
    BinaryTree tree;
};

class AxiomTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::vector<int> nodes = { 1,2,3,4,5,10,20,30,15,25,35 };
        std::vector<std::pair<bool, Node_t*>> output;
        tree.InsertNodes(nodes, output);
    }
    BinaryTree tree;
};

TEST_F(EmptyTree, InsertNode) {
	EXPECT_EQ(tree.GetRoot(), nullptr);

    auto result = tree.InsertNode(10);
    EXPECT_TRUE(result.first);
    EXPECT_EQ(tree.GetRoot()->key,10);

    result = tree.InsertNode(20);
    EXPECT_TRUE(result.first);
    EXPECT_EQ(tree.GetRoot()->key,10);
    EXPECT_EQ(result.second->pParent->key, 10);
    
    result = tree.InsertNode(10);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second->pParent,nullptr);
}

TEST_F(EmptyTree, DeleteNode) {
    EXPECT_FALSE(tree.DeleteNode(10));
}

TEST_F(EmptyTree, FindNode) {
    EXPECT_EQ(tree.FindNode(10),nullptr);
}

TEST_F(NonEmptyTree, InsertNode) {
    EXPECT_EQ(tree.GetRoot()->key, 10);
    EXPECT_EQ(tree.GetRoot()->color,BLACK);

    auto result = tree.InsertNode(10);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(tree.GetRoot(), result.second);
    
    result = tree.InsertNode(11);
    EXPECT_TRUE(result.first);
    EXPECT_EQ(tree.GetRoot()->key, 10);
    EXPECT_EQ(result.second->pParent->key,15);
    EXPECT_EQ(result.second->color, RED);
}

TEST_F(NonEmptyTree, DeleteNode) {
    EXPECT_TRUE(tree.DeleteNode(10));
    EXPECT_EQ(tree.GetRoot()->key, 9);
    int nodes[] = {1,15,2,3,89,85,90,4,7,9};
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(tree.DeleteNode(nodes[i]));
    }
    EXPECT_EQ(tree.GetRoot(),nullptr);
}

TEST_F(NonEmptyTree, FindNode) {
    EXPECT_EQ(tree.FindNode(10),tree.GetRoot());
    EXPECT_EQ(tree.FindNode(999), nullptr);
    EXPECT_EQ(tree.FindNode(2)->pParent->key, 10);
}

TEST_F(AxiomTest, Axiom1) {
    std::vector<BinaryTree::Node_t *> leaves;
    tree.GetLeafNodes(leaves);
    
    for (auto i : leaves) {
        EXPECT_EQ(i->color, BLACK);
    }
}

TEST_F(AxiomTest, Axiom2) {
    std::vector<BinaryTree::Node_t *> nodes;
    tree.GetAllNodes(nodes);

    for (auto i : nodes) {
        if (i->color == RED) {
            if (i->pLeft != NULL)
                EXPECT_EQ(i->pLeft->color, BLACK);
            if (i->pRight != NULL)
                EXPECT_EQ(i->pRight->color, BLACK);
        }
    }
}

TEST_F(AxiomTest, Axiom3) {
    std::vector<BinaryTree::Node_t *> leaves;
    tree.GetLeafNodes(leaves);
    auto root = tree.GetRoot();
    std::vector<int> counts;

    for (auto i : leaves) {
        int c = 0;
        BinaryTree::Node_t * working = i;
        while (i != root) {
            i = i->pParent;
            if (i->color == BLACK) 
                c++;
        }
        counts.push_back(c);
    }

    int ref = counts.back();
    for (int i : counts) {
        EXPECT_EQ(i,ref);
    }
}

/*** Konec souboru black_box_tests.cpp ***/
