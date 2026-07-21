<html lang="en">
      <head>   
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <!-- Bootstrap CSS -->
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
        
        <!-- Google Fonts -->
        <link rel="preconnect" href="https://fonts.googleapis.com">
        <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
        <link href="https://fonts.googleapis.com/css2?family=Lexend+Deca:wght@100..900&family=Sigmar&display=swap" rel="stylesheet">
        <link rel="icon" type="image/x-icon" href="../img/asnm.jpg">
        <link rel="stylesheet" href="../style.css">
        <title>Search</title>
      </head>
      <body>

<header class="navbar">
        <div class="logo" >Adaptive Sports Near Me

        </div>

        <nav>
            <a href="../index.html">Home</a>
            <a hred="./index.html">Search</a>
            <a href="../about/index.html">About</a>
            <a href="#">Contact</a>
            <button class="nav-btn">List Your Program</button>

        </nav>
</header>

<main class="hero">

    

    <div class="hero-content">

        <div class="search-area">

            <h2>Explore our Database</h2>

            <form class="search-form">
                <input
                    type="text"
                    class="search-input"
                    placeholder="Enter specifications..."
                >
                <select name="" id="">
                    <option value="1">🔍 </option>
                </select>

                <button
                    type="submit"
                    class="search-btn">
                    Show Me Programs
                </button>
            </form>

        </div>
        <?php $conn = pg_connect("host=localhost dbname=ASNM user=postgres password=");

$query = "SELECT * FROM ASNMInstances ORDER BY sport_name, instance_name";
$result = pg_query($conn, $query);
?>

       <div class="results">

<?php while ($row = pg_fetch_assoc($result)) { ?>

    <div class="sport-card">

        <h3><?= htmlspecialchars(trim($row['instance_name'])) ?></h3>

        <h4><?= htmlspecialchars(trim($row['sport_name'])) ?></h4>

        <a>
            Ages: <?= $row['instance_age_floor'] ?> - <?= $row['instance_age_ceil'] ?>
        </a>

        <h5>
            <?= htmlspecialchars(trim($row['instance_desc'])) ?>
        </h5>

        <a>
            <?= htmlspecialchars(trim($row['instance_address_line_1'])) ?><br>

            <?= htmlspecialchars(trim($row['instance_city'])) ?>,
            <?= htmlspecialchars(trim($row['instance_state'])) ?>
            <?= htmlspecialchars(trim($row['instance_zip'])) ?>

            <br><br>

            <?= $row['instance_start_month'] ?>/<?= $row['instance_start_day'] ?>/<?= $row['instance_start_year'] ?>
            -

            <?= $row['instance_end_month'] ?>/<?= $row['instance_end_day'] ?>/<?= $row['instance_end_year'] ?>
        </a>

        <h6>

            <?php
            $levels = [];

            if ($row['instance_allows_beginners'])
                $levels[] = "Beginner";

            if ($row['instance_allows_intermediate'])
                $levels[] = "Intermediate";

            if ($row['instance_allows_advanced'])
                $levels[] = "Advanced";

            echo implode(", ", $levels);
            ?>

            <br>

            Cost:
            <?= ($row['instance_cost'] == 0)
                ? "Free"
                : "$" . number_format($row['instance_cost'], 2); ?>

        </h6>

        <a>
            Organization ID:
            <?= htmlspecialchars(trim($row['organization_id'])) ?>
        </a>

        <h6>
            Quality Rating:
            <?= number_format($row['instance_quality'], 1) ?>/5
        </h6>

    </div>

<?php } ?>

</div>

    </div>

</main>

</body>
</html>