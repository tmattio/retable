# ReTable

A sortable selectable *table for ReasonReact.

## Usage

```reason
type president = {
  name: string,
  year: int,
  city: string,
  state: string,
};

module PresidentTable =
  ReTable.Make({
    type t = president;
    let getItemID = president => president.name;
    let columns = [
      ReTable.textColumn("Name", el => el.name),
      ReTable.intColumn("Year", el => el.year),
      ReTable.textColumn("City", el => el.city),
      ReTable.textColumn("State", el => el.state),
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

## Installation

```shell
yarn add bs-retable
```

Then add it to `bsconfig.json`

```json
"bs-dependencies": [
  "bs-retable"
]
```

## Design Decisions

**Q: Why a module type?**

I went through a lot of design phase for the selection of components. The table data is a parametrized type, and so it is impossible to store it in a state. Alternative solutions were to store the IDs of the items, or delegate the selection state to the user. The former is a less-than optimal solution and the latter gives a really bad UX.

## TODO

- [X] Implement selection
- [ ] Implement sorting
- [ ] Implement pagination
- [ ] Implement filtering

**onSelect callback**

When providing selectable data, the callback sends the list of selected items. This appeared to be a good solution, but pose a problem when the parent component is a reducer component, as we need to repopulate the list of selected data