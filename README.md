# reason-table

A sortable selectable *-table for Reason-React.

## Usage

```reason
open ReasonTable;

type president = {
  name: string,
  year: int,
  city: string,
  state: string,
};

module PresidentTable =
  Table.Make({
    type t = president;
    let getItemID = president => president.name;
    let columns = [
      Table.textColumn("Name", el => el.name),
      Table.intColumn("Year", el => el.year),
      Table.textColumn("City", el => el.city),
      Table.textColumn("State", el => el.state),
    ];
  });

let presidents: list(president) = [
  {
    name: "George Washington",
    year: 1732,
    city: "Westmoreland County",
    state: "Virginia",
  },
  {name: "John Adams", year: 1735, city: "Braintree", state: "Massachusetts"},
  {name: "Thomas Jefferson", year: 1743, city: "Shadwell", state: "Virginia"},
];

let component = ReasonReact.statelessComponent(__MODULE__);

let make = (_children: array(ReasonReact.reactElement)) => {
  ...component,
  render: _self => <PresidentTable data=presidents />,
};
```

## TODO

- [X] Implement selection
- [ ] Implement sorting
- [ ] Implement pagination
- [ ] Implement filtering
