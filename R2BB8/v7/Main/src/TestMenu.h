// Test Menu:
#include "./Graphics/Colors.h"
#include "./Page/Page.h"
#include "./Page/Grid.h"
#include "./Page/ButtonItem/h"

Page* p = new Page(1);
Grid* main_grid = new Grid(p, 1,2);
Grid* sub_grids[] = {
  new Grid(2,1),
  new Grid(3,1)
};
main_grid->setItems(sub_grids);
ButtonItem* buttons = {
  new ButtonItem(GREEN, BLACK, WHITE, "Yes"),
  new ButtonItem(RED, BLACK, WHITE, "No"),
  new ButtonItem(BLUE, BLACK, WHITE, "Cancel")
}// **TODO** ADD FUQN TEXT TO THE BUTTONS!!! **TODO** (and TEXT COLOR)
sub_grids[1]->setItems(buttons);

SystemStruct:
{
  DisplayDriver,
  Font,
  Language
}
