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
  render: _self => <PresidentTable data=Data(presidents) />,
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

**Q: Why a union type on the data?**

I have to admit that the API is less elegant with a union type (i.e. `Data | SelectableData`) as a parameter, but other solutions allowed impossible states to happen and it would be a shame to use a language that can forbid impossible states without doing so...

For instance, let's say we accept a parameter `~onSelect: list('a) => unit=?`. If the parameter is `None`, the table is not selectable, otherwise, it is. In this case, we could use two fields in the state and use on or the other depending on the value of `onSelect`, the API would be cleaner, but the code not so much. This is obviously not the only solution, but I found all the solutions I tried without a union type a bit too hacky.

## TODO

- [X] Implement selection
- [ ] Implement sorting
- [ ] Implement pagination
- [ ] Implement filtering
